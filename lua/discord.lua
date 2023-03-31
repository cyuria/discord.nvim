local function CopyFile(old_path, new_path)
    local io = require'io'
    local old_file = io.open(old_path, "rb")
    local new_file = io.open(new_path, "wb")
    local old_file_sz, new_file_sz = 0, 0
    if not old_file or not new_file then
        return false
    end
    while true do
        local block = old_file:read(2^13)
        if not block then
            old_file_sz = old_file:seek( "end" )
            break
        end
        new_file:write(block)
    end
    old_file:close()
    new_file_sz = new_file:seek( "end" )
    new_file:close()
    return new_file_sz == old_file_sz
end


local ffi = require'ffi'
local library_path = (function()
  local dirname = string.sub(debug.getinfo(1).source, 2, #"/lua/discord.lua" * -1)
  if package.config:sub(1, 1) == "\\" then
    return dirname .. "/build/libdiscord-presence.dll"
  else
    return dirname .. "/build/libdiscord-presence.so"
  end
end)()

local function loadCopyC(path)
    -- get the package directory
    local dirname = string.sub(debug.getinfo(1).source, 2, #"/lua/discord.lua" * -1)
    -- create a new path under ${package_root}/bin/
    -- this will contain copies of the binary so that the program can still be compiled
    -- and overwrite the primary /build/ files
    local newpath
    if package.config:sub(1, 1) == "\\" then
        newpath = dirname .. "/bin/libdiscord-presence.dll"
    else
        newpath = dirname .. "/bin/libdiscord-presence.so"
    end
    -- if ${project_root}/bin isn't a directory, make it
    if vim.fn.isdirectory(dirname .. "/bin") == 0 then
        vim.fn.mkdir(dirname .. "/bin", 'p')
    end
    -- copy the files
    if CopyFile(path, newpath) then
        -- return the loaded binary
        return ffi.load(newpath)
    end
    -- only get here if copying failed
    print("Discord: Something went wrong while copying and/or loading library files")
end

local native = {}

-- Declare types for my functions
ffi.cdef [[
    void discordInit();
    void discordShutDown();
    void configMappings(const char*);
    void discordSetFolder(const char*, const char*);
    void discordSetFile(const char*);
    void discordFileNums(const unsigned int, const unsigned int);
    void discordSetExt(const char*);
]]

local DEFAULT_OPTS = {
    usercmd = true,
    autocmd = true,
    initonenter = true,
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

discordPresence.setFolder = function(folder, filename)
    native.discordSetFolder(folder, filename)
end

discordPresence.setFile = function(filename)
    native.discordSetFile(filename)
end

discordPresence.setFileNums = function(currfile, allfiles)
    native.discordFileNums(tonumber(currfile), tonumber(allfiles))
end

discordPresence.setExt = function(ext)
    native.discordSetExt(ext)
end

discordPresence.setup = function(opts)
    opts = opts or DEFAULT_OPTS
    -- load the C++ package
    if not pcall(
        function()
            native = loadCopyC(library_path)
        end) then
        print("Discord.nvim Failed to start because the C++ library could not be copied or is in use")
        return
    end
    local dirname = string.sub(debug.getinfo(1).source, 2, #"/lua/discord.lua" * -1)
    native.configMappings(dirname .. "/langmappings.txt")
    local discordaugroup = vim.api.nvim_create_augroup("discord", {})
    if opts.usercmd then
        vim.api.nvim_create_user_command('DiscordInit', function(args) discordPresence.init() end, {})
        vim.api.nvim_create_user_command('DiscordStop', function(args) discordPresence.stop() end, {})
        vim.api.nvim_create_user_command('DiscordChwd', function(args) discordPresence.setFolder(args.fargs[1], args.fargs[2]) end, { nargs = '*' })
        vim.api.nvim_create_user_command('DiscordFile', function(args) discordPresence.setFile(args.fargs[1]) end, { nargs = 1 })
        vim.api.nvim_create_user_command('DiscordFNum', function(args) discordPresence.setFileNums(args.fargs[1], args.fargs[2]) end, { nargs = '*' })
        vim.api.nvim_create_user_command('DiscordFExt', function(args) discordPresence.setExt(args.fargs[1]) end, { nargs = 1 })
    end
    if opts.autocmd then
        vim.api.nvim_create_autocmd({ "BufEnter" }, {
            group = discordaugroup,
            callback = function(args)
                discordPresence.setFile(vim.fn.fnamemodify(args.file, ':t'))
                discordPresence.setExt(vim.fn.fnamemodify(args.file, ':e'))
                local nrbufs = 0
                for i = 0, vim.fn.tabpagenr('$') do
                    local bufs = vim.fn.tabpagebuflist(i+1)
                    if type(bufs) == "number" then return end
                    nrbufs = nrbufs + #bufs
                end
                print(nrbufs)
                discordPresence.setFileNums(args.buf, nrbufs)
            end
        })
        vim.api.nvim_create_autocmd({ "DirChanged" }, {
            group = discordaugroup,
            callback = function(args)
                local dirname = vim.fn.fnamemodify(vim.fn.getcwd(), ':t')
                local filename = vim.fn.fnamemodify(args.file, ':t')
                discordPresence.setFolder(dirname, filename)
            end
        })
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

