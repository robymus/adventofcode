# Input
input = $<.map{|s|x=s.split(/[ :]+/).map(&:to_i);[x[0],x[1..-1]]}
# Part 1
def possible(all_ops,target, num)
    all_ops.repeated_permutation(num.size-1).any?{|ops|
        a=num[0]
        num[1..-1].zip(ops).each{|x,o|o=='|' ? a=(a.to_s+x.to_s).to_i : o=='+'? a+=x : a*=x}
        a==target
    }
end
p input.select{|x|possible ['+', '*'], x[0], x[1]}.sum(&:first)
# Part 2 (20.17s)
p input.select{|x|possible ['+', '*', '|'], x[0], x[1]}.sum(&:first)
