# Part 1
stones=gets.split.map(&:to_i)
$b=Hash.new
$b[0]=[1]
def blink(i)
    i.flat_map{|x|
        if $b.has_key? x then
            $b[x]
        elsif x.to_s.length % 2 == 0 then
            s=x.to_s
            l=s.length/2
            $b[x] = [s[0...l].to_i, s[l...2*l].to_i]
        else
            $b[x] = [2024*x]
        end
    }
end
s=stones
25.times{s=blink(s)}
p s.size
# Part 2
def blink_t(t)
    nt = Hash.new(0)
    t.each{|x,cnt|
        if $b.has_key? x then
            $b[x].each{|y|nt[y] += cnt}
        elsif x.to_s.length % 2 == 0 then
            s=x.to_s
            l=s.length/2
            $b[x] = [s[0...l].to_i, s[l...2*l].to_i]
            $b[x].each{|y|nt[y] += cnt}
        else
            $b[x] = [2024*x]
            nt[2024*x] += cnt
        end
    }
    nt
end
t=s.tally
50.times{|i|t=blink_t(t)}
p t.values.sum
