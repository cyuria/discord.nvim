# discord.nvim
my own take on the discord rich presence plugin in neovim

![](https://user-images.githubusercontent.com/55673467/219996206-c5a488d9-cf27-4d0f-aa0b-5cbb74a278d7.gif)

## Features

- should be cross-platform
- It doesn't ruin my neovim config
- Easy to use and unobtrusive
- Uses CWD as "project" (And therefore works with [ahmedkhalf/project.nvim](https://github.com/ahmedkhalf/project.nvim), for example)
- Only requires cmake and a C++ compiler, no neovim plugin dependencies

## Installation

### Vim-Plug

`Plug 'Cyuria/discord.nvim', { 'do': 'cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release && cmake --install build --prefix build' }`

### Other

It should be basically the same as any other plugin manager, just make sure to run the following command whenever you install or update:
`cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release && cmake --install build --prefix build`

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
