#/bin/bash
VERSION=`git ls-remote https://github.com/maurodelazeri/mandarinfish | grep refs/tags | grep -oE "[0-9]+\.[0-9]+\.[0-9]+$" | sort --version-sort | tail -n 1` \
    && sh -c "curl -L https://github.com/maurodelazeri/mandarinfish/releases/download/${VERSION}/mandarinfish > /usr/local/bin/mandarinfish" \
    && chmod +x /usr/local/bin/mandarinfish

/usr/local/bin/mandarinfish
