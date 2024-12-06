# Part 1
def safe?(levels)
	diff=levels.each_cons(2).map{|a,b|a-b}
	diff.all?{|x|x>=1&&x<=3} || diff.all?{|x|x>=-3&&x<=-1}
end
records=$<.map{|s|s.split.map(&:to_i)}
p records.count{|r|safe? r}
# Part 2
p records.count{|r|(0...r.size).to_a.any?{|idx|safe?(r[0,idx]+r[idx+1..-1])}}
