#include <cstdlib>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "WorkflowEngine.h"
#include "WorkflowDefinition.h"
#include "TransitionRule.h"
#include "Role.h"
#include "WorkflowInstance.h"
#include "TaskComponent.h"
#include "ApprovalRequest.h"
#include "AuditEvent.h"
#include "DueDateIterator.h"
#include "HumanTask.h"
#include "Types.h"

// ===========================================================================
// Terminal setup and colours
// ===========================================================================

const char *const RESET = "\033[0m";
const char *const BOLD = "\033[1m";
const char *const DIM = "\033[2m";
const char *const RED = "\033[31m";
const char *const GREEN = "\033[32m";
const char *const YELLOW = "\033[33m";
const char *const BLUE = "\033[34m";
const char *const CYAN = "\033[36m";
const char *const MAGENTA = "\033[35m";

/// @brief Prepares the console: ANSI colours, UTF-8 box drawing and a title (Windows only).
void setupTerminal()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitleA("TIVIDY Workflow Manager");
    const DWORD virtualTerminal = 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(out, &mode))
    {
        SetConsoleMode(out, mode | virtualTerminal);
    }
#endif
}

/// @brief Clears the visible screen and its scrollback, then moves the cursor to the top.
void clearScreen()
{
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
}

/// @brief Task status colours: Pending yellow, InProgress blue, Completed green, Cancelled red.
const char *colourForStatus(const std::string &status)
{
    if (status == "Pending")
        return YELLOW;
    if (status == "InProgress")
        return BLUE;
    if (status == "Completed")
        return GREEN;
    if (status == "Cancelled")
        return RED;
    return "";
}

/// @brief Colour for one audit line, based on the event it records.
const char *colourForLogLine(const std::string &line)
{
    if (line.find("TaskDone") != std::string::npos)
        return GREEN;
    if (line.find("TaskStarted") != std::string::npos)
        return BLUE;
    if (line.find("TaskCreated") != std::string::npos)
        return YELLOW;
    if (line.find("TaskCancelled") != std::string::npos)
        return RED;
    if (line.find("Refused") != std::string::npos)
        return RED;
    return "";
}

// ===========================================================================
// Layout: a fixed-width panel drawn with box characters
// ===========================================================================

int screenWidth = 100; // columns in the terminal, updated on every redraw
int screenHeight = 30; // rows in the terminal, updated on every redraw
int INNER = 76;        // width between the left and right borders
int CW = 74;           // width of the text inside a row

/// @brief Reads the terminal size and sizes the panels to fill it.
void refreshLayout()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
    {
        screenWidth = info.srWindow.Right - info.srWindow.Left + 1;
        screenHeight = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
    {
        screenWidth = ws.ws_col;
        screenHeight = ws.ws_row;
    }
#endif
    // Leave a one column margin on each side of the panels.
    int outer = screenWidth - 2;
    if (outer < 60)
        outer = 60;
    INNER = outer - 2;
    CW = INNER - 2;
}

/// @brief Repeats a (possibly multi-byte) piece count times.
std::string repeatPiece(const std::string &piece, int count)
{
    std::string out;
    for (int i = 0; i < count; ++i)
        out += piece;
    return out;
}

/// @brief Cuts text to a width with "..." or pads it with spaces to that width.
std::string fit(const std::string &text, int width)
{
    if (static_cast<int>(text.size()) > width)
    {
        return text.substr(0, width - 3) + "...";
    }
    return text + std::string(width - text.size(), ' ');
}

void panelTop(const std::string &title)
{
    std::string label = title.empty() ? std::string() : " " + title + " ";
    int rest = INNER - static_cast<int>(label.size()) - 1;
    std::cout << u8"╔═" << label << repeatPiece(u8"═", rest) << u8"╗\n";
}

void panelSeparator()
{
    std::cout << u8"╠" << repeatPiece(u8"═", INNER) << u8"╣\n";
}

void panelBottom()
{
    std::cout << u8"╚" << repeatPiece(u8"═", INNER) << u8"╝\n";
}

/// @brief A plain row inside a panel, optionally coloured.
void panelRow(const std::string &text, const char *colour = "")
{
    std::cout << u8"║ " << colour << fit(text, CW) << RESET << u8" ║\n";
}

/// @brief A row whose status tag has its own colour.
void panelRowWithTag(const std::string &prefix, const std::string &colour,
                     const std::string &tag, const std::string &suffix)
{
    std::string visible = prefix + tag + suffix;
    std::string shownSuffix = suffix;
    int overflow = static_cast<int>(visible.size()) - CW;
    if (overflow > 0)
    {
        int keep = static_cast<int>(suffix.size()) - overflow;
        shownSuffix = (keep > 0) ? suffix.substr(0, keep) : "";
    }
    int used = static_cast<int>(prefix.size() + tag.size() + shownSuffix.size());
    std::cout << u8"║ " << prefix << colour << tag << RESET << shownSuffix
              << std::string(CW - used, ' ') << u8" ║\n";
}

// ===========================================================================
// Session: everything the program knows
// ===========================================================================

struct Person
{
    int id;
    std::string name;
    int level;
    std::string roleName;
};

struct Offer
{
    int instanceId;
    ApprovalRequest *request;
    std::string taskName;
};

/// @brief One numbered line on the dashboard: a task, or an approval offer.
struct Item
{
    bool isOffer;
    int instanceId;
    int taskId;
    std::size_t offerIndex;
    std::string label;
    bool needsApproval;
    std::string neededRole;
    bool priority;
};

enum MessageKind
{
    MSG_NONE,
    MSG_INFO,
    MSG_OK,
    MSG_ERROR
};

struct Session
{
    WorkflowEngine engine;
    std::vector<Person> people;
    std::vector<std::string> workflowNames;
    std::vector<int> caseIds;
    std::vector<Offer> offers;
    std::string today;
    MessageKind messageKind;
    std::string message;

    Session() : today("2026-10-28"), messageKind(MSG_NONE) {}
};

void setMessage(Session &s, MessageKind kind, const std::string &text)
{
    s.messageKind = kind;
    s.message = text;
}

std::string roleNameFor(const Session &s, int level)
{
    if (level == 0)
        return "Automated";
    for (std::size_t i = 0; i < s.people.size(); ++i)
    {
        if (s.people[i].level == level)
            return s.people[i].roleName;
    }
    std::ostringstream name;
    name << "Level " << level;
    return name.str();
}

bool hasPerson(const Session &s, int id)
{
    for (std::size_t i = 0; i < s.people.size(); ++i)
    {
        if (s.people[i].id == id)
            return true;
    }
    return false;
}

// ===========================================================================
// Text helpers and input
// ===========================================================================

std::string trim(const std::string &s)
{
    std::size_t begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return "";
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::vector<std::string> splitCsv(const std::string &line)
{
    std::vector<std::string> fields;
    std::string current;
    for (std::size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == ',')
        {
            fields.push_back(trim(current));
            current.clear();
        }
        else
        {
            current += line[i];
        }
    }
    fields.push_back(trim(current));
    return fields;
}

/// @brief Reads a file into lines, skipping blank lines and lines starting with '#'.
bool readLines(const std::string &path, std::vector<std::string> &lines)
{
    std::ifstream in(path.c_str());
    if (!in)
        return false;
    std::string line;
    while (std::getline(in, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        lines.push_back(line);
    }
    return true;
}

std::vector<std::string> parseSubtasks(const std::string &field)
{
    std::vector<std::string> names;
    std::string inner = trim(field);
    if (!inner.empty() && inner[0] == '[')
        inner = inner.substr(1);
    if (!inner.empty() && inner[inner.size() - 1] == ']')
        inner = inner.substr(0, inner.size() - 1);

    std::string current;
    for (std::size_t i = 0; i < inner.size(); ++i)
    {
        if (inner[i] == ';')
        {
            if (!trim(current).empty())
                names.push_back(trim(current));
            current.clear();
        }
        else
        {
            current += inner[i];
        }
    }
    if (!trim(current).empty())
        names.push_back(trim(current));
    return names;
}

/// @brief Shows the message line at the bottom of the screen, then clears it.
void drawFooter(Session &s)
{
    panelSeparator();
    const char *colour = "";
    switch (s.messageKind)
    {
    case MSG_INFO:
        colour = YELLOW;
        break;
    case MSG_OK:
        colour = GREEN;
        break;
    case MSG_ERROR:
        colour = RED;
        break;
    default:
        break;
    }
    panelRow(s.message.empty() ? std::string("Ready.") : s.message, colour);
    panelBottom();
    s.message.clear();
    s.messageKind = MSG_NONE;
}

std::string prompt(const std::string &label)
{
    std::cout << "\033[" << screenHeight << ";1H\033[2K" << CYAN << " " << label << RESET << std::flush;
    std::string line;
    if (!std::getline(std::cin, line))
    {
        std::cout << "\nInput closed. Goodbye.\n";
        std::exit(0);
    }
    return trim(line);
}

int promptInt(const std::string &label)
{
    std::string text = prompt(label);
    if (text.empty())
        return -1;
    return std::atoi(text.c_str());
}

// ===========================================================================
// Screen header
// ===========================================================================

void drawHeader(const Session &s, const std::string &screenTitle)
{
    refreshLayout();
    clearScreen();
    std::cout << u8"╔" << repeatPiece(u8"═", INNER) << u8"╗\n";

    std::string left = "  TIVIDY  Workflow Manager";
    std::string right = "Today " + s.today + "  ";
    std::cout << u8"║" << BOLD << CYAN << fit(left + std::string(INNER - left.size() - right.size(), ' ') + right, INNER)
              << RESET << u8"║\n";

    std::ostringstream stats;
    stats << "  Workflows " << s.workflowNames.size() << "    People " << s.people.size()
          << "    Cases " << s.caseIds.size() << "    Offers " << s.offers.size();
    std::cout << u8"║" << DIM << fit(stats.str(), INNER) << RESET << u8"║\n";

    panelSeparator();
    std::cout << u8"║" << BOLD << fit("  " + screenTitle, INNER) << RESET << u8"║\n";
    panelSeparator();
}

// ===========================================================================
// Loading a workflow file
// ===========================================================================

bool loadWorkflow(Session &s, const std::string &path, std::string &failure)
{
    std::vector<std::string> lines;
    if (!readLines(path, lines))
    {
        failure = "cannot open '" + path + "' (put it in the same folder as the program)";
        return false;
    }

    std::string workflowName;
    std::string startStatus;
    std::vector<std::string> skipped;

    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        std::vector<std::string> f = splitCsv(lines[i]);
        if (f[0] == "workflow" && f.size() >= 3)
        {
            workflowName = f[1];
            startStatus = f[2];
        }
        else if (f[0] == "person" && f.size() >= 5)
        {
            int id = std::atoi(f[1].c_str());
            int level = std::atoi(f[3].c_str());
            if (hasPerson(s, id))
            {
                skipped.push_back(f[2]);
                continue;
            }
            try
            {
                s.engine.addParticipant(id, f[2], level, f[4]);
                Person p = {id, f[2], level, f[4]};
                s.people.push_back(p);
            }
            catch (const std::exception &e)
            {
                failure = e.what();
                return false;
            }
        }
    }

    if (workflowName.empty())
    {
        failure = "the file has no 'workflow,<name>,<start status>' line";
        return false;
    }
    for (std::size_t i = 0; i < s.workflowNames.size(); ++i)
    {
        if (s.workflowNames[i] == workflowName)
        {
            failure = "a workflow called '" + workflowName + "' is already loaded";
            return false;
        }
    }

    WorkflowDefinition *definition = new WorkflowDefinition(workflowName, startStatus);
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        std::vector<std::string> f = splitCsv(lines[i]);
        if (f[0] != "rule" || f.size() < 9)
            continue;

        int level = std::atoi(f[4].c_str());
        Role role(level, roleNameFor(s, level));
        int days = std::atoi(f[7].c_str());
        definition->addRule(TransitionRule(f[1], f[2], f[3], role, f[5], f[6], days, parseSubtasks(f[8])));
    }

    try
    {
        s.engine.registerDefinition(definition);
    }
    catch (const std::exception &e)
    {
        delete definition;
        failure = e.what();
        return false;
    }

    s.workflowNames.push_back(workflowName);
    if (!skipped.empty())
    {
        failure = "loaded '" + workflowName + "'; already in the roster, skipped: " + skipped[0];
        if (skipped.size() > 1)
            failure += " and " + std::to_string(skipped.size() - 1) + " more";
        return true;
    }
    failure.clear();
    return true;
}

// ===========================================================================
// Starting cases
// ===========================================================================

std::string chooseWorkflow(Session &s)
{
    if (s.workflowNames.empty())
    {
        setMessage(s, MSG_ERROR, "No workflow is loaded yet. Load one first (option 1).");
        return "";
    }
    if (s.workflowNames.size() == 1)
        return s.workflowNames[0];

    panelRow("Loaded workflows:");
    for (std::size_t i = 0; i < s.workflowNames.size(); ++i)
    {
        panelRow("  " + std::to_string(i + 1) + ")  " + s.workflowNames[i]);
    }
    int choice = promptInt("Which workflow? ");
    if (choice < 1 || choice > static_cast<int>(s.workflowNames.size()))
    {
        setMessage(s, MSG_ERROR, "That is not a workflow number.");
        return "";
    }
    return s.workflowNames[choice - 1];
}

bool startCase(Session &s, const std::string &workflow, const std::vector<std::string> &header,
               const std::vector<std::string> &values, std::string &failure)
{
    DataMap data;
    for (std::size_t i = 0; i < header.size() && i < values.size(); ++i)
    {
        data[header[i]] = values[i];
    }
    try
    {
        WorkflowInstance *instance = s.engine.startInstance(workflow, values[0], data);
        if (instance == nullptr)
        {
            failure = "the workflow could not start the case";
            return false;
        }
        s.caseIds.push_back(instance->getId());
        return true;
    }
    catch (const std::exception &e)
    {
        failure = e.what();
        return false;
    }
}

void typedCaseScreen(Session &s)
{
    drawHeader(s, "NEW CASE  -  type the data");
    panelTop("HOW THIS WORKS");
    panelRow("Type the field names on one line, separated by commas.");
    panelRow("Then type the values on the next line, in the same order.");
    panelRow("The first value becomes the case name. Example:  claimNumber,fraudFlag");
    panelBottom();
    std::string workflow = chooseWorkflow(s);
    if (workflow.empty())
    {
        drawFooter(s);
        prompt("Press Enter to continue...");
        return;
    }

    std::string header = prompt("Field names, comma separated (e.g. claimNumber,fraudFlag): ");
    std::string values = prompt("Values, same order (e.g. CLM-1,yes): ");
    std::vector<std::string> h = splitCsv(header);
    std::vector<std::string> v = splitCsv(values);

    if (h.size() != v.size() || v.empty())
    {
        setMessage(s, MSG_ERROR, "The number of field names and values must match.");
        return;
    }
    std::string failure;
    if (startCase(s, workflow, h, v, failure))
    {
        setMessage(s, MSG_OK, "Started case " + std::to_string(s.caseIds.back()) + " '" + v[0] + "'.");
    }
    else
    {
        setMessage(s, MSG_ERROR, failure);
    }
}

void casesFromFileScreen(Session &s)
{
    drawHeader(s, "NEW CASES  -  read from a file");
    panelTop("HOW THIS WORKS");
    panelRow("The file must be in the same folder as the program.");
    panelRow("First line: field names, comma separated. Each line after it is one case.");
    panelBottom();
    std::string workflow = chooseWorkflow(s);
    if (workflow.empty())
        return;

    std::string path = prompt("Cases file name: ");
    std::vector<std::string> lines;
    if (!readLines(path, lines) || lines.size() < 2)
    {
        setMessage(s, MSG_ERROR, "The file needs a header line and at least one case.");
        return;
    }

    std::vector<std::string> header = splitCsv(lines[0]);
    int started = 0;
    std::string lastFailure;
    for (std::size_t i = 1; i < lines.size(); ++i)
    {
        std::vector<std::string> values = splitCsv(lines[i]);
        std::string failure;
        if (values.size() != header.size())
        {
            lastFailure = "line " + std::to_string(i + 1) + " has the wrong number of values";
            continue;
        }
        if (startCase(s, workflow, header, values, failure))
        {
            ++started;
        }
        else
        {
            lastFailure = failure;
        }
    }

    std::ostringstream summary;
    summary << "Started " << started << " case(s).";
    if (!lastFailure.empty())
    {
        setMessage(s, MSG_ERROR, summary.str() + " Problem: " + lastFailure);
    }
    else
    {
        setMessage(s, MSG_OK, summary.str());
    }
}

// ===========================================================================
// Shared lookups
// ===========================================================================

std::string nameOf(const Session &s, int id)
{
    for (std::size_t i = 0; i < s.people.size(); ++i)
    {
        if (s.people[i].id == id)
            return s.people[i].name;
    }
    return "unknown";
}

int levelOf(const Session &s, int id)
{
    for (std::size_t i = 0; i < s.people.size(); ++i)
    {
        if (s.people[i].id == id)
            return s.people[i].level;
    }
    return 0;
}

/// @brief The case ids as text, for example "1, 2, 3", or a hint if there are none.
std::string caseListText(const Session &s)
{
    if (s.caseIds.empty())
        return "none yet (start one from option 2 or 3)";
    std::string out;
    for (std::size_t i = 0; i < s.caseIds.size(); ++i)
    {
        if (i > 0)
            out += ", ";
        out += std::to_string(s.caseIds[i]);
    }
    return out;
}

// ===========================================================================
// Help
// ===========================================================================

void helpScreen(Session &s)
{
    drawHeader(s, "HELP");

    panelTop("MAIN MENU");
    panelRow("1  Load a workflow file        Reads a .txt file from the program's folder.");
    panelRow("2  Start a case by typing      You type the field names and values.");
    panelRow("3  Start cases from a file     Reads a .csv-style file, one case per line.");
    panelRow("4  Person dashboard            Shows what one person can do right now.");
    panelRow("5  Case history                Shows every event that happened in a case.");
    panelRow("6  Cancel a case               Stops a case and cancels its open tasks.");
    panelRow("7  Save snapshot               Saves the state of every case.");
    panelRow("8  Restore snapshot            Puts the cases back to a saved state.");
    panelBottom();

    panelTop("DASHBOARD ACTIONS  (type the item number, then the letter)");
    panelRow("1s   start a task you can do            Pending -> InProgress");
    panelRow("1c   complete a task you have started   InProgress -> Completed");
    panelRow("1r   ask for approval on a task above your level");
    panelRow("1m   mark or unmark a task as priority  (!! and listed first)");
    panelRow("1a   accept an approval offer           the offer becomes your task");
    panelRow("1p   pass an approval offer up          use this if it is above your level");
    panelRow("0    switch to another person           ?  show this help");
    panelBottom();

    panelTop("COLOURS");
    panelRow("Pending      yellow", YELLOW);
    panelRow("InProgress   blue", BLUE);
    panelRow("Completed    green", GREEN);
    panelRow("Cancelled    red", RED);
    panelRow("NEEDS APPROVAL   magenta: the person is below the task's level", MAGENTA);
    panelRow("!!  priority task (listed first)", BOLD);
    panelBottom();

    prompt("Press Enter to return.");
}

// ===========================================================================
// Person dashboard
// ===========================================================================

/// @brief Draws a person's open tasks, approval offers and the action guide. Returns the numbered items.
///
/// Tasks are ordered by DueDateIterator: priority tasks first, then earliest due date.
std::vector<Item> drawDashboard(Session &s, int personId, int personLevel)
{
    std::vector<Item> items;
    int number = 1;

    // Collect the person's open tasks and remember which case each belongs to.
    std::vector<TaskComponent *> mine;
    std::map<TaskComponent *, int> caseOf;
    for (std::size_t c = 0; c < s.caseIds.size(); ++c)
    {
        WorkflowInstance *instance = s.engine.findInstance(s.caseIds[c]);
        if (instance == nullptr)
            continue;
        std::vector<TaskComponent *> open = instance->openLeafTasks();
        for (std::size_t t = 0; t < open.size(); ++t)
        {
            if (open[t]->getAssignee() == personId)
            {
                mine.push_back(open[t]);
                caseOf[open[t]] = instance->getId();
            }
        }
    }

    DueDateIterator ordered(mine);

    panelTop("TASKS FOR " + nameOf(s, personId) + "   (level " + std::to_string(personLevel) + ")");
    while (ordered.hasNext())
    {
        TaskComponent *task = ordered.next();
        if (task == nullptr)
            break;

        std::string status = task->getStatus();
        int neededLevel = task->getRequiredRole().getLevel();
        bool needsApproval = personLevel < neededLevel;
        std::string neededRole = roleNameFor(s, neededLevel);
        const HumanTask *human = dynamic_cast<const HumanTask *>(task);
        bool priority = (human != nullptr) && human->isPriority();
        int caseId = caseOf[task];

        std::string numberText = std::to_string(number);
        std::string due = task->getDueDate().empty() ? std::string("none") : task->getDueDate();
        std::string prefix = "  " + numberText + std::string(4 - numberText.size(), ' ') +
                             (priority ? "!! " : "   ");
        std::string tag = status + std::string(13 - status.size(), ' ');
        std::string suffix = task->getName() + "  case " + std::to_string(caseId) + "  due " + due;
        panelRowWithTag(prefix, colourForStatus(status), tag, suffix);

        if (needsApproval)
        {
            panelRow("        NEEDS APPROVAL: " + neededRole + " level " + std::to_string(neededLevel) +
                         ".  Type " + numberText + "r to ask for approval.",
                     MAGENTA);
        }
        else if (status == "InProgress")
        {
            panelRow("        You can do this. Type " + numberText + "c to complete it.", DIM);
        }
        else
        {
            panelRow("        You can do this. Type " + numberText + "s to start it.", DIM);
        }

        Item item = {false, caseId, task->getId(), 0, task->getName(), needsApproval, neededRole, priority};
        items.push_back(item);
        ++number;
    }
    if (items.empty())
    {
        panelRow("  Nothing to do right now.", DIM);
    }

    panelSeparator();
    bool anyOffer = false;
    for (std::size_t o = 0; o < s.offers.size(); ++o)
    {
        const Offer &offer = s.offers[o];
        if (!offer.request->isOpen() || offer.request->getHolderId() != personId)
            continue;

        std::string numberText = std::to_string(number);
        panelRow("  " + numberText + "    APPROVAL OFFER: " + offer.taskName + "  case " +
                     std::to_string(offer.instanceId),
                 YELLOW);
        panelRow("        Someone asked you to take this. Type " + numberText +
                     "a to accept it, or " + numberText + "p to pass it up.",
                 YELLOW);

        Item item = {true, offer.instanceId, 0, o, offer.taskName, false, "", false};
        items.push_back(item);
        ++number;
        anyOffer = true;
    }
    if (!anyOffer)
    {
        panelRow("  No approval offers for this person.", DIM);
    }

    panelSeparator();
    panelRow("HOW TO ACT", CYAN);
    panelRow("  1s  start a task you can do       1c  complete a task you started");
    panelRow("  1r  ask for approval (magenta)    1m  mark or unmark priority (!!)");
    panelRow("  1a  accept an offer               1p  pass an offer up");
    panelRow("  0 or Enter: switch person         ?   help");
    panelBottom();
    return items;
}

/// @brief Parses "3s" into item 3 and action 's'.
bool parseItemAction(const std::string &input, int &number, char &action)
{
    std::string digits;
    std::size_t i = 0;
    while (i < input.size() && input[i] >= '0' && input[i] <= '9')
        digits += input[i++];
    if (digits.empty() || i + 1 != input.size())
        return false;
    number = std::atoi(digits.c_str());
    action = input[i];
    return true;
}

void actOnItem(Session &s, int personId, const Item &item, int number, char action)
{
    std::string num = std::to_string(number);

    if (item.isOffer)
    {
        const Offer &offer = s.offers[item.offerIndex];
        try
        {
            if (action == 'a')
            {
                s.engine.acceptRequest(offer.instanceId, offer.request, personId);
                setMessage(s, MSG_OK, "Accepted '" + offer.taskName + "'. It is now yours.");
            }
            else if (action == 'p')
            {
                s.engine.passUpRequest(offer.instanceId, offer.request, personId);
                setMessage(s, MSG_INFO, "Passed '" + offer.taskName + "' up to the next level.");
            }
            else
            {
                setMessage(s, MSG_ERROR, "Item " + num + " is an approval offer. Type " + num + "a to accept it, or " + num + "p to pass it up.");
            }
        }
        catch (const std::exception &e)
        {
            setMessage(s, MSG_ERROR, e.what());
        }
        return;
    }

    if (action == 'm')
    {
        try
        {
            s.engine.setPriority(item.instanceId, item.taskId, !item.priority);
            setMessage(s, MSG_OK, item.priority ? "'" + item.label + "' is no longer a priority." : "'" + item.label + "' is now a priority.");
        }
        catch (const std::exception &e)
        {
            setMessage(s, MSG_ERROR, e.what());
        }
        return;
    }
    if (action != 's' && action != 'c' && action != 'r')
    {
        setMessage(s, MSG_ERROR, "Item " + num + " is a task. Use " + num + "s (start), " + num + "c (complete), " + num + "r (ask for approval) or " + num + "m (priority).");
        return;
    }
    if (action == 'r' && !item.needsApproval)
    {
        setMessage(s, MSG_INFO, "You can do '" + item.label + "' yourself. Type " + num + "s to start it, then " + num + "c to complete it.");
        return;
    }
    if (action != 'r' && item.needsApproval)
    {
        setMessage(s, MSG_ERROR, "'" + item.label + "' NEEDS APPROVAL from " + item.neededRole + ". You cannot do it yourself. Type " + num + "r to ask for approval.");
        return;
    }

    try
    {
        if (action == 's')
        {
            s.engine.startTask(item.instanceId, item.taskId, personId);
            setMessage(s, MSG_OK, "Started '" + item.label + "'.");
        }
        else if (action == 'c')
        {
            s.engine.completeTask(item.instanceId, item.taskId, personId);
            setMessage(s, MSG_OK, "Completed '" + item.label + "'.");
        }
        else
        {
            ApprovalRequest *request = s.engine.requestApproval(item.instanceId, item.taskId, personId);
            if (request != nullptr)
            {
                Offer offer = {item.instanceId, request, item.label};
                s.offers.push_back(offer);
            }
            setMessage(s, MSG_INFO, "Approval requested for '" + item.label + "'. Waiting for " + item.neededRole + ".");
        }
    }
    catch (const std::exception &e)
    {
        setMessage(s, MSG_ERROR, e.what());
    }
}

void dashboardScreen(Session &s)
{
    if (s.people.empty())
    {
        setMessage(s, MSG_ERROR, "No people are loaded yet. Load a workflow file first (option 1).");
        return;
    }

    int personId = -1;
    while (true)
    {
        if (personId < 0)
        {
            drawHeader(s, "PERSON DASHBOARD  -  choose a person");
            panelTop("PEOPLE");
            panelRow("Pick a person. Their level decides whether they can act alone or need approval.", DIM);
            panelSeparator();
            for (std::size_t i = 0; i < s.people.size(); ++i)
            {
                const Person &p = s.people[i];
                panelRow("  " + std::to_string(p.id) + "    " + fit(p.name, 14) + " " +
                         fit(p.roleName, 22) + " level " + std::to_string(p.level));
            }
            panelBottom();
            drawFooter(s);
            int choice = promptInt("Person id (0 = back to menu): ");
            if (choice == 0)
                return;
            if (!hasPerson(s, choice))
            {
                setMessage(s, MSG_ERROR, "No person has id " + std::to_string(choice) + ". Choose one from the list.");
                continue;
            }
            personId = choice;
            continue;
        }

        drawHeader(s, "PERSON DASHBOARD  -  " + nameOf(s, personId));
        std::vector<Item> items = drawDashboard(s, personId, levelOf(s, personId));
        drawFooter(s);

        std::string input = prompt("Action, e.g. 1s = start item 1   (? help, 0 switch person): ");
        if (input == "0" || input.empty())
        {
            personId = -1;
            continue;
        }
        if (input == "?")
        {
            helpScreen(s);
            continue;
        }

        int number = 0;
        char action = 0;
        if (!parseItemAction(input, number, action))
        {
            setMessage(s, MSG_ERROR, "Type the item number followed by one letter, for example 1s. Type ? for help.");
            continue;
        }
        if (number < 1 || number > static_cast<int>(items.size()))
        {
            setMessage(s, MSG_ERROR, "There is no item " + std::to_string(number) + " on this list. Item numbers are in the first column.");
            continue;
        }
        actOnItem(s, personId, items[number - 1], number, action);
    }
}

// ===========================================================================
// History, cancelling, snapshots, loading
// ===========================================================================

void historyScreen(Session &s)
{
    drawHeader(s, "CASE HISTORY");
    panelTop("CASES");
    panelRow("Case ids: " + caseListText(s));
    panelBottom();

    int id = promptInt("Case id to view (0 = back): ");
    if (id == 0)
        return;
    WorkflowInstance *instance = s.engine.findInstance(id);
    if (instance == nullptr)
    {
        setMessage(s, MSG_ERROR, "There is no case " + std::to_string(id) + ". Use one of the ids listed above.");
        return;
    }

    drawHeader(s, "CASE HISTORY  -  case " + std::to_string(id));
    panelTop(instance->getName());
    panelRow("Status: " + instance->getStatus(), colourForStatus(instance->getStatus()));
    panelSeparator();
    const std::vector<AuditEvent> &log = instance->getAuditLog();
    if (log.empty())
    {
        panelRow("No events yet.", DIM);
    }
    for (std::size_t i = 0; i < log.size(); ++i)
    {
        std::string line = log[i].format();
        panelRow(line, colourForLogLine(line));
    }
    panelBottom();
    prompt("Press Enter to return.");
}

void cancelCaseScreen(Session &s)
{
    drawHeader(s, "CANCEL A CASE");
    panelTop("CASES");
    if (s.caseIds.empty())
    {
        panelRow("There are no cases yet.", DIM);
    }
    for (std::size_t i = 0; i < s.caseIds.size(); ++i)
    {
        WorkflowInstance *instance = s.engine.findInstance(s.caseIds[i]);
        if (instance == nullptr)
            continue;
        std::string status = instance->getStatus();
        panelRowWithTag("  " + std::to_string(instance->getId()) + "    ", colourForStatus(status),
                        status, "   " + instance->getName());
    }
    panelBottom();
    panelRow("", "");
    int id = promptInt("Case id to cancel (0 = back): ");
    if (id == 0)
        return;
    try
    {
        s.engine.cancelInstance(id);
        setMessage(s, MSG_OK, "Case " + std::to_string(id) + " cancelled. Its open tasks were cancelled too.");
    }
    catch (const std::exception &e)
    {
        setMessage(s, MSG_ERROR, e.what());
    }
}

// void saveSnapshotScreen(Session& s)
// {
//     try {
//         s.engine.saveSnapshot();
//         setMessage(s, MSG_OK, "Snapshot saved. Use option 8 to go back to it later.");
//     } catch (const std::exception& e) {
//         setMessage(s, MSG_ERROR, e.what());
//     }
// }

void saveSnapshotScreen(Session &s)
{
    try
    {
        s.engine.saveSnapshot();
        int index = s.engine.snapshotCount() - 1;
        setMessage(s, MSG_OK, "Snapshot " + std::to_string(index) + " saved. Use option 8 and type " + std::to_string(index) + " to restore it.");
    }
    catch (const std::exception &e)
    {
        setMessage(s, MSG_ERROR, e.what());
    }
}

// void restoreSnapshotScreen(Session& s)
// {
//     drawHeader(s, "RESTORE A SNAPSHOT");
//     panelTop("SNAPSHOTS");
//     panelRow("Snapshots are numbered from 0, in the order they were saved.");
//     panelRow("Restoring puts every case back to how it was when the snapshot was saved.", DIM);
//     panelBottom();
//     int index = promptInt("Snapshot number (0 is the first one saved): ");
//     try {
//         s.engine.restoreSnapshot(index);
//         setMessage(s, MSG_OK, "Snapshot " + std::to_string(index) + " restored.");
//     } catch (const std::exception& e) {
//         setMessage(s, MSG_ERROR, "Snapshot " + std::to_string(index) + " does not exist. Save one first (option 7).");
//     }
// }

void restoreSnapshotScreen(Session &s)
{
    drawHeader(s, "RESTORE A SNAPSHOT");
    panelTop("SNAPSHOTS");
    int count = s.engine.snapshotCount();
    if (count == 0)
    {
        panelRow("No snapshots saved yet. Use option 7 first.", RED);
    }
    else
    {
        panelRow("Saved snapshots: " + std::to_string(count) +
                 "  (valid numbers: 0 to " + std::to_string(count - 1) + ")");
    }
    panelRow("Restoring puts every case back to how it was when the snapshot was saved.", DIM);
    panelBottom();
    if (count == 0)
    {
        prompt("Press Enter to return.");
        return;
    }

    int index = promptInt("Snapshot number (0 is the first one saved): ");
    if (index < 0)
    {
        setMessage(s, MSG_ERROR, "Enter a number from 0 to " + std::to_string(count - 1) + ".");
        return;
    }
    try
    {
        s.engine.restoreSnapshot(index);
        setMessage(s, MSG_OK, "Snapshot " + std::to_string(index) + " restored.");
    }
    catch (const std::exception &)
    {
        setMessage(s, MSG_ERROR, "There is no snapshot " + std::to_string(index) + ". Valid numbers: 0 to " + std::to_string(count - 1) + ".");
    }
}

void loadWorkflowScreen(Session &s)
{
    drawHeader(s, "LOAD A WORKFLOW");
    panelTop("HOW THIS WORKS");
    panelRow("Type the file name exactly, for example insurance.txt or manufacturing.txt.");
    panelRow("The file must be in the same folder as the program.");
    panelBottom();
    std::string path = prompt("Workflow file name: ");
    std::string failure;
    bool ok = loadWorkflow(s, path, failure);
    if (ok && failure.empty())
    {
        setMessage(s, MSG_OK, "Loaded '" + path + "'. Its people and process map are ready.");
    }
    else if (ok)
    {
        setMessage(s, MSG_INFO, failure);
    }
    else
    {
        setMessage(s, MSG_ERROR, failure);
    }
}

// ===========================================================================
// Home screen
// ===========================================================================

void homeScreen(Session &s)
{
    drawHeader(s, "MAIN MENU");

    panelTop("ACTIONS");
    panelRow("  1    Load a workflow file");
    panelRow("  2    Start a case by typing its data");
    panelRow("  3    Start cases from a file");
    panelRow("  4    Person dashboard          what a person can do right now");
    panelRow("  5    Case history              every event in one case");
    panelRow("  6    Cancel a case");
    panelRow("  7    Save snapshot");
    panelRow("  8    Restore snapshot");
    panelRow("  9    Help                      what each option and code means");
    panelRow("  0    Quit");
    panelBottom();

    drawFooter(s);
}

int main()
{
    setupTerminal();
    Session session;

    drawHeader(session, "WELCOME");
    panelTop("SETUP");
    panelRow("Enter today's date so due dates are worked out from it.");
    panelRow("Press Enter to use 2026-10-28.", DIM);
    panelBottom();
    std::string today = prompt("Today's date (YYYY-MM-DD): ");
    if (!today.empty())
        session.today = today;
    session.engine.setToday(session.today);

    while (true)
    {
        homeScreen(session);
        int choice = promptInt("Select an option (1-9, or 0 to quit): ");

        switch (choice)
        {
        case 1:
            loadWorkflowScreen(session);
            break;
        case 2:
            typedCaseScreen(session);
            break;
        case 3:
            casesFromFileScreen(session);
            break;
        case 4:
            dashboardScreen(session);
            break;
        case 5:
            historyScreen(session);
            break;
        case 6:
            cancelCaseScreen(session);
            break;
        case 7:
            saveSnapshotScreen(session);
            break;
        case 8:
            restoreSnapshotScreen(session);
            break;
        case 9:
            helpScreen(session);
            break;
        case 0:
            clearScreen();
            std::cout << "Goodbye.\n";
            return 0;
        default:
            setMessage(session, MSG_ERROR, "Choose a number from the menu (1-9, or 0 to quit).");
        }
    }
}