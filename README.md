# pocket-tool

List, archive, delete, or favorite multiple articles in your [Pocket](https://getpocket.com) list at once based on their tags and/or added date.

## Examples

List all articles added since 2018-04-08:  
`pocket-tool list --since 2018-04-08`

Archive each article tagged with both "ifttt" and "nyt", but not "politics":  
`pocket-tool archive -t ifttt -t nyt -T politics`

Delete all articles added more than 4 days ago:  
`date -v-4d +%Y-%m-%d | xargs -IX pocket-tool delete --before X`

Favorite all archived articles without the tag "ifttt":  
`pocket-tool favorite --archived --without-tag ifttt`

## Usage

`pocket-tool [--version] [--help] <command> [<options>]`

### Options

<table>
<tbody><tr>
<td>
<code>-q --quiet</code>
</td>
<td>
Don’t print article list.
</td>
</tr>
<tr>
<td>
<code>-m --markdown</code>
</td>
<td>
Print articles as links in a Markdown list format.
</td>
</tr>
</tbody></table>

### Commands

<table>
<tbody><tr>
<td>
<code>list [&lt;criteria&gt;]</code>
</td>
<td>
List articles matching <code>&lt;criteria&gt;</code>. This command doesn’t modify any articles.
</td>
</tr>
<tr>
<td>
<code>favorite   [&lt;criteria&gt;]</code><br>
<code>unfavorite [&lt;criteria&gt;]</code>
</td>
<td>
Favorite or unfavorite articles matching <code>&lt;criteria&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>archive   [&lt;criteria&gt;]</code><br>
<code>unarchive [&lt;criteria&gt;]</code>
</td>
<td>
Archive or unarchive articles matching <code>&lt;criteria&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>delete [&lt;criteria&gt;]</code>
</td>
<td>
Delete articles matching <code>&lt;criteria&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>auth &lt;consumer-key&gt;</code>
</td>
<td>
See <a href="#authentication">Authentication</a>.
</td>
</tr>
</tbody></table>

### Criteria

These options are used to limit a command to only act on certain articles. Only articles that match every given criteria are acted on.

<table>
<tbody><tr>
<td>
<code>-t --with-tag &lt;tag&gt;</code>
</td>
<td>
Match articles with <code>&lt;tag&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>-T --without-tag &lt;tag&gt;</code>
</td>
<td>
Do not match articles with <code>&lt;tag&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>-a --archived</code>
</td>
<td>
Match archived articles.
</td>
</tr>
<tr>
<td>
<code>-A --not-archived</code>
</td>
<td>
Match articles in “My List” (articles that aren’t archived).
</td>
</tr>
<tr>
<td>
<code>-f --favorited</code>
</td>
<td>
Match favorited articles.
</td>
</tr>
<tr>
<td>
<code>-F --not-favorited</code>
</td>
<td>
Match articles that aren’t favorited.
</td>
</tr>
<tr>
<td>
<code>-b --before &lt;YYYY-MM-DD&gt;</code>
</td>
<td>
Match articles added before <code>&lt;YYYY-MM-DD&gt;</code>.
</td>
</tr>
<tr>
<td>
<code>-s --since  &lt;YYYY-MM-DD&gt;</code>
</td>
<td>
Match articles added since <code>&lt;YYYY-MM-DD&gt;</code>.
</td>
</tr>
</tbody></table>

If `--archived`/`--not-archived` is not used, both archived and not archived articles are matched.

If `--favorited`/`--not-favorited` is not used, both favorited and non favorited articles are matched.

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

## Authentication

Before you can use `pocket-tool`, you must [create a Pocket consumer key](https://getpocket.com/developer/apps/new), and authenticate the app by running `pocket-tool auth <consumer-key>`. This will create a private access token, which is stored along with your consumer key in `~/.pocket-tool`.

## Dependencies

- [getopt.h](https://www.gnu.org/software/gnulib/manual/html_node/getopt_002eh.html)
- [jsmn](https://github.com/zserge/jsmn)
- [libcurl](https://curl.haxx.se/libcurl/)

On macOS, the only one of these that doesn't come with the OS is jsmn.
