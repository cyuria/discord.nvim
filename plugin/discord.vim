" Fallback to setting up the plugin automatically
if !exists("g:discord_has_setup")
lua << EOF
    
    local Discord = require("discord").setup()
    
EOF
endif

