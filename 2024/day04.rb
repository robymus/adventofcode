# Part 1
def cnt(grid, word)
    w=grid[0].size
    h=grid.size
    l=word.size
    r=h.times.sum{|y|w.times.sum{|x|[-1,0,1].sum{|dx|[-1,0,1].count{|dy|l.times.all?{|i|
        (0...w)===x+dx*i&&(0...h)===y+dy*i&&grid[y+dy*i][x+dx*i]==word[i]
    }}}}}
end
grid=$<.map{|l|l.strip.chars}
p cnt(grid, "XMAS")
# Part 2
p 1.upto(grid.size-2).sum{|y|1.upto(grid[0].size-2).count{|x|
    grid[y][x]=='A' &&
    [grid[y-1][x-1],grid[y+1][x+1]].sort == ['M', 'S'] &&
    [grid[y+1][x-1],grid[y-1][x+1]].sort == ['M', 'S']
}}
