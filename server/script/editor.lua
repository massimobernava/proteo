XSIZE = 10
YSIZE = 20
TILE_BASE_X = 40
TILE_BASE_Y = 60
TILE_WIDTH = 30
TILE_HEIGHT = 30

BLOCK_FALL_INTERVAL = 0.5
KEYBOARD_EVENT_INTERVAL = 0.06
REMOVE_ROWS_ANIMATION_SPEED = TILE_WIDTH * 1.5
REMOVE_ROWS_ANIMATION_INTERVAL = 0

BORDER_WIDTH_RATIO = 0.25
BRIGHT_ALPHA = 255
NORMAL_ALPHA = 190
DARK_ALPHA = 90

-- -----------------------------------------------------

_ = require 'underscore'

function inspect(obj)
   if type(obj) ~= 'table' then
      return tostring(obj)
   else
      local pairs = _.map(_.keys(obj), 
                          function (k)
                             return tostring(k) .. " = " .. inspect(obj[k])
                          end)
      return "{ " .. _.join(pairs, ", ") .. " }"
   end
end

function clone(obj)
   if type(obj) ~= 'table' then
      return obj
   else
      local cloned = {}
      for k, v in pairs(obj) do
         cloned[k] = clone(v)
      end
      return cloned
   end
end

function concat(arr1, arr2)
   local result = {}
   for i, v in ipairs(arr1) do
      table.insert(result, v)
   end
   for i, v in ipairs(arr2) do
      table.insert(result, v)
   end
   return result
end

-- ------------------------------------------------------

Huor = {}
HuorQueue = {}

function Huor.poll()
   if not _.is_empty(HuorQueue) then
      local huor = _.shift(HuorQueue)
      if love.timer.getMicroTime() >= huor.waketime then
         huor.work()
      else
         _.push(HuorQueue, huor)
      end
   end
end

function Huor.add(steptime, work)
   local huor = { waketime = love.timer.getMicroTime() + steptime,
                  work = work }
   _.push(HuorQueue, huor)
end

-- ------------------------------------------------------

StockBlocks = { { start_position = { x = 4, y = 1 },
                  color = { 175, 175, 0 },
                  symbol = "T",
                  transformations = { { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 0, y = -1 } },
                                      { { x = 0, y = 0 },
                                        { x = 0, y = -1 },
                                        { x = 0, y = 1 },
                                        { x = 1, y = 0 } },
                                      { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 0, y = 1 } },
                                      { { x = 0, y = 0 },
                                        { x = 0, y = -1 },
                                        { x = 0, y = 1 },
                                        { x = -1, y = 0 } } } },
                { start_position = { x = 4, y = 1 },
                  color = { 0, 255, 0 },
                  symbol = "Z",
                  transformations = { { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = -1 },
                                        { x = 1, y = -1 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 1, y = 1 } } } },
                { start_position = { x = 4, y = 1 }, 
                  color = { 100, 100, 175 },
                  symbol = "S",
                  transformations = { { { x = 1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = -1 },
                                        { x = -1, y = -1 } },
                                      { { x = 0, y = 1 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 1, y = -1 } } } },
                { start_position = { x = 4, y = 0 }, 
                  color = { 255, 255, 100 },
                  symbol = "J",
                  transformations = { { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = -1, y = 1 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = 1, y = 1 } },
                                      { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 1, y = -1 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = -1, y = -1 } } } },
                { start_position = { x = 4, y = 0 },
                  color = { 255, 0, 255 },
                  symbol = "L",
                  transformations = { { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 1, y = 1 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = 1, y = -1 } },
                                      { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = -1, y = -1 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = -1, y = 1 } } } },
                { start_position = { x = 4, y = 0 },
                  color = { 255, 0, 0 },
                  symbol = "I",
                  transformations = { { { x = -1, y = 0 },
                                        { x = 0, y = 0 },
                                        { x = 1, y = 0 },
                                        { x = 2, y = 0 } },
                                      { { x = 0, y = -1 },
                                        { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = 0, y = 2 } } } },
                { start_position = { x = 4, y = 0 },
                  color = { 0, 0, 255 },
                  symbol = "[]",
                  transformations = { { { x = 0, y = 0 },
                                        { x = 0, y = 1 },
                                        { x = 1, y = 0 },
                                        { x = 1, y = 1 } } } }
             }

-- ------------------------------------------------------

Tetris = {}
Tetris.state = {}
Tetris.state_update = {}
Tetris.huors = {}
Tetris.util = {}
Tetris.graphics = {}

function Tetris.init()
   Tetris.state.keybuffer = {}
   Tetris.state.occupied_matrix = {}
   Tetris.state.passive_blocks = {}
   Tetris.state_update.make_new_active_block()

   Huor.add(BLOCK_FALL_INTERVAL, Tetris.huors.active_block_fall)
   Huor.add(KEYBOARD_EVENT_INTERVAL, Tetris.huors.process_keyboard_input)
end

function Tetris.huors.active_block_fall()
   local block = Tetris.state.active_block
   local fallen_block = clone(block)
   fallen_block.position.y = block.position.y + 1
   if Tetris.util.is_valid_pos(fallen_block) then
      Tetris.state.active_block = fallen_block
      Huor.add(BLOCK_FALL_INTERVAL, Tetris.huors.active_block_fall)
   else
      Tetris.state_update.finish_fall()
      if Tetris.state_update.make_new_active_block() == 'OK' then         
         Huor.add(BLOCK_FALL_INTERVAL, Tetris.huors.active_block_fall)
      else
         Tetris.state.game_over = true
      end
   end
end

function Tetris.huors.process_keyboard_input()
   local function detected(key)
      return love.keyboard.isDown(key) or _.include(Tetris.state.keybuffer, key)
   end

   local next_block = clone(Tetris.state.active_block)
   if detected('left') then
      next_block.position.x = next_block.position.x - 1
   elseif detected('right') then
      next_block.position.x = next_block.position.x + 1
   elseif detected('down') then
      next_block.position.y = next_block.position.y + 1
   elseif _.include(Tetris.state.keybuffer, 'up') then 
      -- do not need continuous rotate
      local sblock = StockBlocks[next_block.stock_index]
      local transformation_count = #sblock.transformations
      next_block.transformation_index = 
         next_block.transformation_index % transformation_count + 1
      next_block = Tetris.util.adjust_position_after_rotation(next_block)
   end
   Tetris.state.keybuffer = {}

   if Tetris.util.is_valid_pos(next_block) then
      Tetris.state.active_block = next_block
   end -- else do nothing

   if not Tetris.state.game_over then
      Huor.add(KEYBOARD_EVENT_INTERVAL, Tetris.huors.process_keyboard_input)
   end
end

function Tetris.huors.remove_rows_animate()
   if Tetris.state.remove_rows_animation_lines then
      local longest_line = _.max(Tetris.state.remove_rows_animation_lines, 
                                 function (line)
                                    return line.finish.x
                                 end)
      if longest_line.finish.x > XSIZE * TILE_WIDTH + TILE_BASE_X then
         -- animation finish
         Tetris.state.remove_rows_animation_lines = nil
         Tetris.state_update.remove_rows(Tetris.state.full_rows)
      else
         _.each(Tetris.state.remove_rows_animation_lines,
                function (line)
                   line.finish.x = line.finish.x + REMOVE_ROWS_ANIMATION_SPEED
                end)
         Huor.add(REMOVE_ROWS_ANIMATION_INTERVAL, Tetris.huors.remove_rows_animate)
      end
   end -- else do nothing
end

function Tetris.state_update.finish_fall()
   local block = Tetris.state.active_block
   block.tiles = Tetris.util.get_actual_tile_positions(block)
   block.position = nil
   _.map(block.tiles, Tetris.state_update.set_occupied_matrix)
   _.push(Tetris.state.passive_blocks, block)   

   local full_rows = Tetris.util.get_full_rows()
   if not _.is_empty(full_rows) then
      Tetris.state_update.setup_remove_rows_animation(full_rows)
      Tetris.state.full_rows = full_rows
      Huor.add(REMOVE_ROWS_ANIMATION_INTERVAL, Tetris.huors.remove_rows_animate)
   end
end

function Tetris.state_update.make_new_active_block()
   local block = Tetris.util.get_stock_block(math.random(#StockBlocks))

   if not Tetris.util.is_valid_pos(block) then 
      -- default position is not valid, very crowded
      local higher_pos_block = clone(block)
      higher_pos_block.position.y = higher_pos_block.position.y - 1
      Tetris.state.active_block = higher_pos_block
      return 'game over'
   else
      Tetris.state.active_block = block

      local lower_pos_block = clone(block)
      lower_pos_block.position.y = lower_pos_block.position.y + 1 
      if Tetris.util.is_valid_pos(lower_pos_block) then
         return 'OK'
      else
         return 'game over'
      end
   end
end

function Tetris.state_update.remove_rows(row_indices)
   _.each(row_indices, Tetris.state_update.remove_row)
end

function Tetris.state_update.remove_row(y)
   _.each(Tetris.state.passive_blocks, 
          function (pblock) 
             pblock.tiles = _.reject(pblock.tiles, 
                                     function (tile)
                                        return tile.y == y
                                     end)
             _.each(pblock.tiles,
                    function (tile)
                       if tile.y < y then
                          tile.y = tile.y + 1
                       end
                    end)
          end)

   Tetris.state_update.rebuild_occupied_matrix()
end

function Tetris.state_update.set_occupied_matrix(tile)
   local i = tile.x * YSIZE + tile.y + 1
   Tetris.state.occupied_matrix[i] = true
end

function Tetris.state_update.unset_occupied_matrix(tile)
   local i = tile.x * YSIZE + tile.y + 1
   Tetris.state.occupied_matrix[i] = false
end

function Tetris.state_update.rebuild_occupied_matrix()
   Tetris.state.occupied_matrix = {}

   _.each(Tetris.state.passive_blocks, 
          function (pblock)
             _.each(pblock.tiles, Tetris.state_update.set_occupied_matrix)
          end)
end

function Tetris.state_update.setup_remove_rows_animation(row_indices)
   local function get_row_lines(row_index)
      local range = _.range(row_index * TILE_HEIGHT + TILE_BASE_Y,
                            (row_index + 1) * TILE_HEIGHT + TILE_BASE_Y):to_array()
      return _.map(range, 
                   function (y)
                      local offset = math.random(REMOVE_ROWS_ANIMATION_SPEED) - 1
                      return { start = { x = TILE_BASE_X, y = y },
                               finish = { x = TILE_BASE_X + offset, y = y } }
                   end)
   end

   local row_lines = _.map(row_indices, get_row_lines)

   Tetris.state.remove_rows_animation_lines = _.reduce(row_lines, {}, concat)
end

function Tetris.util.is_occupied(tile)
   local i = tile.x * YSIZE + tile.y + 1
   return Tetris.state.occupied_matrix[i]
end

function Tetris.util.get_actual_tile_positions(block)
   local pos = block.position
   if pos == nil then -- passive block
      return block.tiles
   else -- active block
      local sblock = StockBlocks[block.stock_index]
      local tiles = sblock.transformations[block.transformation_index]
      return _.map(tiles, function (tile)
                             return { x = tile.x + pos.x, 
                                      y = tile.y + pos.y }
                          end)
   end
end

function Tetris.util.is_valid_pos(block)
   local actual_positions = Tetris.util.get_actual_tile_positions(block)
   return _.all(actual_positions, function (tile)
                                     return tile.x >= 0 and tile.x < XSIZE and
                                        tile.y >= 0 and tile.y < YSIZE and
                                        (not Tetris.util.is_occupied(tile))
                                  end)
end

function Tetris.util.get_stock_block(i)
   local sblock = StockBlocks[i]

   return { stock_index = i,
            position = sblock.start_position,
            transformation_index = 1 }
end

function Tetris.util.get_full_rows()
   local function is_full_row(y)
      return _.all(_.range(0, XSIZE - 1):to_array(),
                   function (x) 
                      return Tetris.util.is_occupied({x = x, y = y}) 
                   end)
   end

   return _.filter(_.range(0, YSIZE - 1):to_array(), is_full_row)
end

function Tetris.util.adjust_position_after_rotation(block)
   if Tetris.util.is_valid_pos(block) then
      return block
   end

   local left_block = clone(block)
   left_block.position.x = left_block.position.x - 1
   if Tetris.util.is_valid_pos(left_block) then
      return left_block
   end

   local right_block = clone(block)
   right_block.position.x = right_block.position.x + 1
   if Tetris.util.is_valid_pos(right_block) then
      return right_block
   end

   local below_block = clone(block)
   below_block.position.y = below_block.position.y + 1
   if Tetris.util.is_valid_pos(below_block) then
      return below_block
   end

   -- patch for "straight stick" in most right pos
   sblock = StockBlocks[block.stock_index]
   if sblock.symbol == "I" then
      local left_left_block = clone(block)
      left_left_block.position.x = left_left_block.position.x - 2
      if Tetris.util.is_valid_pos(left_left_block) then
         return left_left_block
      end
   end

   return block -- cannot find a possible valid pos, returns an
                -- invalid pos, which will be prevented in consecutive
                -- guard statements.
end

function Tetris.graphics.draw_fps()
   love.graphics.print("FPS: " .. tostring(love.timer.getFPS()), 1, 1)
end

function Tetris.graphics.draw_active_block()
   c = StockBlocks[Tetris.state.active_block.stock_index].color
   love.graphics.setColor(c[1], c[2], c[3], 255)
   Tetris.graphics.draw_block(Tetris.state.active_block)
   love.graphics.setColor(255, 255, 255, 255)
end

function Tetris.graphics.draw_passive_blocks()
   love.graphics.setColor(150, 150, 200, 255)
   _.each(Tetris.state.passive_blocks, Tetris.graphics.draw_block)
   love.graphics.setColor(255, 255, 255, 255)
end

function Tetris.graphics.draw_tile(tile)
   local x0 = tile.x * TILE_WIDTH + TILE_BASE_X
   local y0 = tile.y * TILE_HEIGHT + TILE_BASE_Y
   local w = TILE_WIDTH - 1
   local h = TILE_HEIGHT - 1
   local w1 = w * BORDER_WIDTH_RATIO
   local h1 = h * BORDER_WIDTH_RATIO
   local w3 = w - w1
   local h3 = h - h1

   cr, cg, cb, ca = love.graphics.getColor()

   -- draw center
   love.graphics.setColor(cr, cg, cb, NORMAL_ALPHA)
   love.graphics.rectangle('fill', x0 + w1, y0 + h1, w3 - w1, w3 - w1)
   
   -- draw top border
   love.graphics.setColor(cr, cg, cb, BRIGHT_ALPHA)
   love.graphics.quad('fill', x0, y0, x0 + w, y0, 
                      x0 + w3, y0 + h1, x0 + w1, y0 + h1)

   -- draw right border
   love.graphics.quad('fill', x0 + w3, y0 + h1, x0 + w, y0,
                      x0 + w, y0 + h, x0 + w3, y0 + h3)

   -- draw left border
   love.graphics.setColor(cr, cg, cb, DARK_ALPHA)
   love.graphics.quad('fill', x0, y0, x0 + w1, y0 + h1, 
                      x0 + w1, y0 + h3, x0, y0 + h)

   -- draw botton border
   love.graphics.quad('fill', x0 + w1, y0 + h3, x0 + w3, y0 + h3, 
                      x0 + w, y0 + h, x0, y0 + h)

   love.graphics.setColor(cr, cg, cb, ca)
end

function Tetris.graphics.draw_block(block)
   local tiles = Tetris.util.get_actual_tile_positions(block)

   _.each(tiles, Tetris.graphics.draw_tile)

   -- merge adjacent tiles

   local function includes(tiles, tile)
      for i, t in ipairs(tiles) do
         if t.x == tile.x and t.y == tile.y then
            return true
         end
      end

      return false
   end

   local w = TILE_WIDTH - 1
   local h = TILE_HEIGHT - 1
   local w1 = w * BORDER_WIDTH_RATIO
   local h1 = h * BORDER_WIDTH_RATIO
   local w3 = w - w1
   local h3 = h - h1
   local w2 = w1 * 2 + 1
   local h2 = h1 * 2 + 1

   cr, cg, cb, ca = love.graphics.getColor()

   _.each(tiles, function (tile)
                    local x0 = tile.x * TILE_WIDTH + TILE_BASE_X
                    local y0 = tile.y * TILE_HEIGHT + TILE_BASE_Y
                    if includes(tiles, { x = tile.x + 1, y = tile.y }) then
                       -- merge right
                       love.graphics.setColor(0, 0, 0, 255) -- clear
                       love.graphics.rectangle('fill',
                                               x0 + w3, y0, w2, h)
                       love.graphics.setColor(cr, cg, cb, NORMAL_ALPHA) -- center
                       love.graphics.rectangle('fill',
                                               x0 + w3, y0 + h1, w2, h3 - h1)
                       love.graphics.setColor(cr, cg, cb, BRIGHT_ALPHA) -- top
                       love.graphics.rectangle('fill', 
                                               x0 + w3, y0, w2, h1)
                       love.graphics.setColor(cr, cg, cb, DARK_ALPHA) -- bottom
                       love.graphics.rectangle('fill', 
                                               x0 + w3, y0 + h3, w2, h1)
                    end
                    if includes(tiles, { x = tile.x, y = tile.y + 1 }) then
                       -- merge down
                       love.graphics.setColor(0, 0, 0, 255) -- clear
                       love.graphics.rectangle('fill',
                                               x0, y0 + h3, w, h2)
                       love.graphics.setColor(cr, cg, cb, NORMAL_ALPHA) -- center
                       love.graphics.rectangle('fill', 
                                               x0 + w1, y0 + h3, w3 - w1, h2)
                       love.graphics.setColor(cr, cg, cb, BRIGHT_ALPHA) -- right
                       love.graphics.rectangle('fill', 
                                               x0 + w3, y0 + h3, w1, h2)
                       love.graphics.setColor(cr, cg, cb, DARK_ALPHA) -- left
                       love.graphics.rectangle('fill', 
                                               x0, y0 + h3, w1, h2)
                    end
                 end)

   _.each(tiles, function (tile)
                    local x0 = tile.x * TILE_WIDTH + TILE_BASE_X
                    local y0 = tile.y * TILE_HEIGHT + TILE_BASE_Y
                    if includes(tiles, { x = tile.x + 1, y = tile.y }) and 
                       includes(tiles, { x = tile.x, y = tile.y + 1}) then
                       -- patch shadow when both right and down are merged
                       love.graphics.setColor(0, 0, 0, 255) -- clear
                       love.graphics.rectangle('fill',
                                               x0 + w3, y0 + h3, w1, h1)
                       love.graphics.setColor(cr, cg, cb, BRIGHT_ALPHA) -- right
                       love.graphics.triangle('fill', x0 + w3, y0 + h3,
                                              x0 + w3, y0 + h, x0 + w, y0 + h)
                       love.graphics.setColor(cr, cg, cb, DARK_ALPHA) -- bottom
                       love.graphics.triangle('fill', x0 + w3, y0 + h3,
                                              x0 + w, y0 + h3, x0 + w, y0 + h)
                    end
                    if includes(tiles, { x = tile.x - 1, y = tile.y }) and 
                       includes(tiles, { x = tile.x, y = tile.y - 1}) then
                       -- patch shadow when both left and top are merged
                       love.graphics.setColor(0, 0, 0, 255) -- clear
                       love.graphics.rectangle('fill',
                                               x0, y0, w1, h1)
                       love.graphics.setColor(cr, cg, cb, BRIGHT_ALPHA) -- top
                       love.graphics.triangle('fill', x0, y0,
                                              x0, y0 + h1, x0 + w1, y0 + h1)
                       love.graphics.setColor(cr, cg, cb, DARK_ALPHA) -- left
                       love.graphics.triangle('fill', x0, y0,
                                              x0 + w1, y0, x0 + w1, y0 + h1)
                    end
                 end)

   love.graphics.setColor(cr, cg, cb, ca)
end

function Tetris.graphics.draw_border()
   love.graphics.rectangle('line', TILE_BASE_X, TILE_BASE_Y,
                           TILE_WIDTH * XSIZE,
                           TILE_HEIGHT * YSIZE)
end

function Tetris.graphics.draw_game_over()
   love.graphics.print("game over", 80, 10)
end

function Tetris.graphics.draw_remove_rows_animation_lines()
   if Tetris.state.remove_rows_animation_lines then
      local r, g, b, a = love.graphics.getColor()
      love.graphics.setColor(0, 0, 0, 255)
      _.each(Tetris.state.remove_rows_animation_lines, 
             function (line)
                love.graphics.line(line.start.x, line.start.y,
                                   line.finish.x, line.finish.y)
             end)
      love.graphics.setColor(r, g, b, a)
   end
end

function Tetris.render()
   Tetris.graphics.draw_border()

   Tetris.graphics.draw_active_block()
   Tetris.graphics.draw_passive_blocks()
   Tetris.graphics.draw_remove_rows_animation_lines()

   Tetris.graphics.draw_fps()
   if Tetris.state.game_over then
      Tetris.graphics.draw_game_over()
   end
end

-- ------------------------------------------------------

function love.load()
   love.graphics.setMode(1024, 768)
   Tetris.init()
end

function love.keypressed(key, unicode)
   if key == 'escape' then
      love.event.quit()
   elseif _.include({ 'up', 'down', 'left', 'right' }, key) then
      _.push(Tetris.state.keybuffer, key)
   end
end

function love.update()
   Huor.poll()
end

function love.draw()
   Tetris.render()
end