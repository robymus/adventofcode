# Input
antennas=Hash.new{|h,k| h[k]=[]}
i=[*$<]
h=i.size
w=i[0].strip.size
i.each_with_index{|line,y|
    line.strip.chars.each_with_index{|c,x|antennas[c] << [x,y] if c != '.'}
}
# Part 1
p antennas.values
    .flat_map{|coords|coords.combination(2).flat_map{|a,b|[ [ 2*a[0]-b[0], 2*a[1]-b[1] ] , [ 2*b[0]-a[0], 2*b[1]-a[1] ] ]}}
    .select{|c|(0...w)===c[0] && (0...h)===c[1]}
    .uniq.size
# Part 2
p antennas.values.flat_map{|coords|coords.combination(2).flat_map{|a,b|
        n=[]
        dx,dy=b[0]-a[0],b[1]-a[1]
        x,y=a[0],a[1]
        while (0...w)===x && (0...h)===y do
            n << [x,y]; x-=dx; y-=dy
        end
        x,y=b[0],b[1]
        while (0...w)===x && (0...h)===y do
            n << [x,y]; x+=dx; y+=dy
        end
        n
    }}.uniq.size
