FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN g++ -std=c++11 -O2 -Wall -Wextra *.cpp -o myapp

# Runtime image
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --no-create-home appuser

WORKDIR /app

COPY --from=builder /app/myapp ./myapp

USER appuser

CMD ["./myapp"]

