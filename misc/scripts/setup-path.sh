#! /bin/echo You shouldn't execute this file, instead do: source
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )" # find script dir
DIR="$( cd "$DIR" && cd "../../" && pwd)" # goes up to the top dir
export POK_PATH="$DIR"
