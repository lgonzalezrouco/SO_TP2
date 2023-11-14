# Check if docker SO_2 exists
if [ ! "$(docker ps -a -q -f name=SO_2)" ]; then
    # Create docker SO_2
    docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name SO_2 agodio/itba-so:2.0
fi

docker start SO_2
docker exec -it SO_2 make clean -C/root/Toolchain
docker exec -it SO_2 make clean -C/root/
docker exec -it SO_2 make -C/root/Toolchain
if [[ "$1" = "-b" ]]; then
    docker exec -it SO_2 env GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01" make buddy -C/root/
else
    docker exec -it SO_2 env GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01" make -C/root/
fi
# docker exec -it SO_2 chmod -R g+wx /root/
bash run.sh
docker exec -it SO_2 make clean -C/root/