local ffi = require'ffi'
local library_path = (function()
  local dirname = string.sub(debug.getinfo(1).source, 2, #"/discord.lua" * -1)
  if package.config:sub(1, 1) == "\\" then
    return dirname .. "../build/libdiscord-presence.dll"
  else
    return dirname .. "../build/libdiscord-presence.so"
  end
end)()
local native = ffi.load(library_path)

local DEFAULT_OPTS = {
    usercmd = true,
    autocmd = false,
    initialise = true,
    -- Do not turn last option off or there is a good chance the plugin will break.
    -- Restarting **should** fix any problems though
    quitonleave = true,
}

local discordPresence = {}

discordPresence.init = function()
    native.discordInit()
end

discordPresence.stop = function()
    native.discordShutDown()
end

discordPresence.setFolder = function(foldername, newfilename)
    native.discordSetFolder(foldername, newfilename)
end

discordPresence.setFile = function(filename)
    native.discordSetFile(filename)
end

discordPresence.setFileNums = function(currfile, totalfiles)
    native.discordSetFileNums(currfile, totalfiles)
end

discordPresence.setup = function(opts)
    opts = opts or DEFAULT_OPTS
    local discordaugroup = vim.api.nvim_create_augroup("discord", {})
    if opts.usercmd then
        vim.api.nvim_create_user_command('DiscordInit', discordPresence.init, {})
        vim.api.nvim_create_user_command('DiscordStop', discordPresence.stop, {})
        vim.api.nvim_create_user_command('DiscordChwd', discordPresence.setFolder, { nargs = 2 })
        vim.api.nvim_create_user_command('DiscordFile', discordPresence.setFile, { nargs = 1 })
        vim.api.nvim_create_user_command('DiscordFNum', discordPresence.setFileNums, { nargs = 2 })
    end
    if opts.autocmd then
        vim.api.nvim_create_autocmd({ "BufEnter" }, {
            group = discordaugroup,
            callback = function(args)
                discordPresence.setFile(vim.fn.fnamemodify(args.file, ':t'))
                discordPresence.setFileNums()
            end
        })
        -- BufEnter DirChanged
    end
    if opts.initonenter then
        vim.api.nvim_create_autocmd({ "VimEnter" }, {
            group = discordaugroup,
            callback = discordPresence.init
        })
    end
    if opts.quitonleave then
        vim.api.nvim_create_autocmd({ "VimLeavePre" }, {
            group = discordaugroup,
            callback = discordPresence.stop
        })
    end
end

return discordPresence

