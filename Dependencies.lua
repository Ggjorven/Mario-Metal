------------------------------------------------------------------------------
-- Utils
------------------------------------------------------------------------------
local function local_require(path)
	return dofile(path)
end

local function this_directory()
    local str = debug.getinfo(2, "S").source:sub(2)
	local path = str:match("(.*/)")
    return path:gsub("\\", "/") -- Replace \\ with /
end

function append_to_table(dest, value)
	if type(value) == "table" then
		for _, v in ipairs(value) do
        	table.insert(dest, v)
    	end
    else
		table.insert(dest, value)
    end

	return dest
end

function remove_from_table(dest, filter)
    for i = #dest, 1, -1 do  -- Iterate backwards
        local value = dest[i]

		-- Note: Allows lua patterns
        if string.find(value, filter) ~= nil then
            table.remove(dest, i)
        end
    end

	return dest
end

function copy_table(tbl)
    if type(tbl) ~= "table" then 
		return tbl 
	end

    local copy = {}
    for k, v in pairs(tbl) do
        copy[k] = copy_table(v)
    end
	
    return copy
end
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Graphics API flag
------------------------------------------------------------------------------
OBSIDIAN_GRAPHICS_API = "vulkan"
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Dependencies
------------------------------------------------------------------------------
local Dependencies =
{
	-- Internal Dependencies
    Nano = 
    {
        IncludeDir = this_directory() .. "vendor/Nano/Nano/Nano/include"
    },

	Obsidian = 
    {
        -- Note: This is dumb (I know), but it looks the cleanest for now.
        -- We should just load once.
        IncludeDir = local_require("vendor/Obsidian/Obsidian/Dependencies.lua").Obsidian.IncludeDir,
        LibName = local_require("vendor/Obsidian/Obsidian/Dependencies.lua").Obsidian.LibName,
        PostBuildCommands = local_require("vendor/Obsidian/Obsidian/Dependencies.lua").Obsidian.PostBuildCommands,
    }
}

-- Note: Since these dependencies have their own Nano we remove those includedirs
-- and just use the Nano that is a submodule to this repository.
remove_from_table(Dependencies.Obsidian.IncludeDir, "vendor/Nano/Nano")
------------------------------------------------------------------------------

return Dependencies