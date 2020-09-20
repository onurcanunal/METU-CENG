select 
	a3."name" as name,
	count(distinct a2.author_id) as collab_count
from 
	authored a,
	authored a2,
	author a3
where
	a.pub_id = a2.pub_id 
	and a.author_id <> a2.author_id 
	and a.author_id = a3.author_id 
group by 
	a.author_id,
	a3."name" 
order by 
	count(distinct a2.author_id) desc,
	a3."name" 
limit
	1000