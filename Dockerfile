FROM wiiuenv/devkitppc:20220917

COPY --from=wiiuenv/wiiupluginsystem:20220924 /artifacts $DEVKITPRO

WORKDIR project
