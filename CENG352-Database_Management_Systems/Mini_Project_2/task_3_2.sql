select
	a."name" as author_name,
	count(*) as pub_count
from 
	author a,
	authored a2,
	article a3 
where
	a.author_id = a2.author_id 
	and a2.pub_id = a3.pub_id
	and a3.journal like '%IEEE%'
group by 
	a.author_id,
	a."name"
order by 
	count(*) desc,
	a."name" 
limit 
	50