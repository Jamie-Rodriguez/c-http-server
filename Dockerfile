FROM silkeh/clang:19 AS build
WORKDIR /c-http-server/
COPY . .
RUN make release

FROM scratch AS final
COPY --from=build --chmod=755 /c-http-server/bin/server /bin/server
EXPOSE 8080
ENTRYPOINT ["/bin/server"]
