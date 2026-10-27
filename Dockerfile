FROM gcc:11
WORKDIR /app
COPY . .
RUN make clean && make
CMD ["./TIVIDY"]