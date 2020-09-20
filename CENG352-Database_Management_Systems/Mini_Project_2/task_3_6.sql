select
	tmp2."year",
	a3."name",
	count(*)
from
	(select
		tmp."year",
		max(tmp.pub_count) as max_pub_count
	from
		(select 
			p."year",
			count(*) as pub_count
		from 
			"publication" p,
			authored a 
		where
			p.pub_id = a.pub_id 
			and p."year" >= 1940
			and p."year" <= 1990
		group by 
			a.author_id,
			p."year") as tmp
	group by 
		tmp."year") as tmp2,
	"publication" p2,
	authored a2,
	author a3
where 
	p2.pub_id = a2.pub_id 
	and p2."year" >= 1940
	and p2."year" <= 1990
	and p2."year" = tmp2."year"
	and a3.author_id = a2.author_id 
group by 
	a2.author_id,
	a3."name",
	tmp2."year",
	tmp2.max_pub_count
having 
	count(*) = tmp2.max_pub_count
order by 
	tmp2."year",
	a3."name" 
	