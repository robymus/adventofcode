# Part 1
ordering=[]
input=[]
is_input=false
$<.each{|line|
    l=line.strip.split(/[,|]/).map(&:to_i)
    !l[0] ? is_input = true : is_input ? input.push(l) : ordering.push(l)
}
ordered=->(i){ordering.all?{|b,a|!(bi=i.index(b))||!(ai=i.index(a))||bi<ai}}
p input.select(&ordered).sum{|i|i[i.size/2]}
# Part 2
o=ordering.flat_map{|x|[[x,1],[x.reverse,-1]]}.to_h
p input.reject(&ordered).map{|i|i.sort{|a,b|o[[a,b]]||0}}.sum{|i|i[i.size/2]}
