select 
	year_table."year" || '-' || year_table."year" + 10 as decade,
	sum(year_table.cnt)
	over 
		(rows between current row and 9 following) total
from 
	(select
		p."year",
		count(*) as cnt
	from 
		"publication" p
	where
		p."year" >= '1940'
	group by 
		p."year"
	order by 
		p."year") as year_table

	