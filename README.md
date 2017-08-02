# pocket-tool

Archive, delete, or favorite multiple articles in your [Pocket](https://getpocket.com) list at once based on their tags and/or added date.

```
Usage:

    pocket-tool [--version] [--help] <command>

Commands:

    list [<criteria>]          List articles matching <criteria>. This command
                               doesn't modify any articles.

    favorite   [<criteria>]    Favorite articles matching <criteria>.
    unfavorite [<criteria>]    Unfavorite articles matching <criteria>.

    archive   [<criteria>]     Archive articles matching <criteria>.
    unarchive [<criteria>]     Unarchive articles matching <criteria>.

    delete [<criteria>]        Delete articles matching <criteria>.

    auth <consumer-key>        Create a private access token.  Your key-token
                               pair will be stored in '~/.pocket-tool' and used
                               for subsequent calls to the Pocket API. To get a
                               consumer key, follow the instructions at
                               https://getpocket.com/developer/apps/new

Criteria:

    These options are used to limit a command to only act on certain articles.
    Only articles that match every given criteria are acted on.

    -t --with-tag    <tag>     Match articles with <tag>.
    -T --without-tag <tag>     Do not match articles with <tag>.

    -a --archived              Match archived articles.
    -A --not-archived          Match articles in "My List" (articles that aren't
                               archived).

    If --archived/--not-archived is not used, both archived and not archived
    articles are matched.

    -f --favorited             Match favorited articles.
    -F --not-favorited         Match articles that aren't favorited.

    If --favorited/--not-favorited is not used, both favorited and non favorited
    articles are matched.

    -b --before <YYYY-MM-DD>   Match articles added before <YYYY-MM-DD>.
    -s --since  <YYYY-MM-DD>   Match articles added since <YYYY-MM-DD>.

Examples:

    Archive each article tagged with both 'ifttt' and 'nyt', but not 'politics':

        pocket-tool archive -t foo -t bar -T politics
```

## Install with [Homebrew](https://brew.sh)

```
$ brew install ernstwi/tap/pocket-tool
```

## Install manually

1. Clone this repo.
2. Clone [zserge/jsmn](https://github.com/zserge/jsmn).
3. `cd` into the jsmn directory.
4. Run `make CFLAGS=-DJSMN_PARENT_LINKS`.
5. Copy the files `jsmn.h` and `libjsmn.a` to the `pocket-tool` directory created in step 1.
6. `cd` into `pocket-tool`.
7. Run `make`.
8. Run `make install` to move the built binary to `/usr/local/bin/pocket-tool`.

## Authenticate

Before you can use `pocket-tool`, you must [create a Pocket consumer key](https://getpocket.com/developer/apps/new), and authenticate the app by running `pocket-tool auth <consumer-key>`. This will create a private access token, which is stored along with your consumer key in `~/.pocket-tool`.

## Dependencies

- [getopt.h](https://www.gnu.org/software/gnulib/manual/html_node/getopt_002eh.html)
- [jsmn](https://github.com/zserge/jsmn)
- [libcurl](https://curl.haxx.se/libcurl/)

On macOS, the only one of these that doesn't come with the OS is jsmn.
