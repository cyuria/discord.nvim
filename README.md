# discord.nvim
my own take on the discord rich presence plugin in neovim

![](https://user-images.githubusercontent.com/55673467/219996206-c5a488d9-cf27-4d0f-aa0b-5cbb74a278d7.gif)

## Features

- It should be cross-platform (only tested on windows 10 currently)
- It doesn't ruin my neovim config
- Easy to use and unobtrusive
- Uses CWD as "project" (And therefore works with plugins like [ahmedkhalf/project.nvim](https://github.com/ahmedkhalf/project.nvim), for example)
- Only requires cmake and a C++ compiler, no neovim plugin dependencies

## Installation

### Vim-Plug

```vim
Plug 'Cyuria/discord.nvim', { 'do': 'cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release && cmake --install build --prefix build' }
```

### Other

It should be basically the same as any other plugin manager, just make sure to run the following command whenever you install or update:
```shell
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release && cmake --install build --prefix build
```

## Usage

Almost everything is done automagically, but you do need to call `require'discord'.setup()` to add all the autocmds and user commands.

### setup

You need to run `require'discord'.setup()` for the plugin to start doing anything.
`setup` takes an optional table argument, default initialised with the following values:
```lua
local DEFAULT_OPTS = {
    usercmd = true, -- setup user commands
    autocmd = true, -- setup autocommands for updating on file change
    initonenter = true, -- setup autocommand to start on VimEnter
    quitonleave = true, -- setup autocommand to stop on VimLeavePre (important for cleanup stuff, just leave as true regardless)
}
```

### Caveat

Won't work if discord is started after neovim, start discord and then reload neovim

### commands

#### Vim

- `:DiscordInit` | Start talking to discord (not called automatically)
  - No Arguments
- `:DiscordStop` | Shutdown discord, do this for cleanup, it doesn't break anything if discord hasn't been initialised yet
  - No Arguments
- `:DiscordChwd` | Manually change displayed working directory, has no effect on actual working directory
  - Two arguments, new folder name and new filename
  - Automatically resets elapsed time
- `:DiscordFile` | Manually change the displayed file, has no effect on neovim
  - One argument, new file name
- `:DiscordFNum` | Manually change the "number of files" value shown in discord "(x of y)"
  - Two arguments, current file number and total number of files
- `:DiscordFExt` | Manually change the file extension displayed by discord
  - One argument, new file extension

#### Lua

Assumes ```local discordPresence = require'discord'``` present

- `discordPresence.setup()` | Setup plugin
- `discordPresence.init()` | Start talking to discord
- `discordPresence.stop()` | Stop talking to discord (also performs cleanup)
- `discordPresence.setFolder(folder, filename)` | Set a new folder and filename to be displayed
  - Automatically resets elapsed time
- `discordPresence.setFile(filename)` | Set the file to be displayed
- `discordPresence.setFileNums(currentfile, totalfilenum)` | Set the "number of files" value shown in discord "(currentfile of totalfilenum)"
- `discordPresence.setExt(fileext)` | Set the file extension to be used

## Why I Made It

I was using [andweeb/presence.nvim](https://github.com/andweeb/presence.nvim) for this exact purpose, but it had a weird
issue where neovim would hang randomly for up to thirty seconds after closing. I haven't had this issue with my plugin,
also I wanted to write a neovim plugin, otherwise I probably would have used another alternative.

Also I prefer the discord presentation layout in this plugin (naturally, I made it)

## Credits, Inspiration and Alternatives

I copy pasted almost the entire CMakeLists.txt from the one at [nvim-telescope/telescope-fzf-native.nvim](https://github.com/nvim-telescope/telescope-fzf-native.nvim)
I also copied some other parts of the code from there.

I was previously using [andweeb/presence.nvim](https://github.com/andweeb/presence.nvim) and took inspiration as well as the
list of file extension to language mapping data from there.

A large chunk of the code comes from [discord-rpc](https://github.com/discord/discord-rpc), which is now deprecated, but
still works fine and I don't want to learn or use the entire discord SDK for this.
