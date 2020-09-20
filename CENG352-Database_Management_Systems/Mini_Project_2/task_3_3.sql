select
	a."name" as name,
	count(*) as pub_count
from 
	author a,
	authored a2,
	article a3 
where
	a.author_id = a2.author_id 
	and a2.pub_id = a3.pub_id
	and a3.journal = 'IEEE Trans. Wireless Communications'
	and not exists
		(select 
			*
		from 
			authored a4,
			article a5 
		where
			a4.pub_id = a5.pub_id 
			and a4.author_id = a.author_id
			and a5.journal = 'IEEE Wireless Commun. Letters')
group by 
	a.author_id,
	a."name"
having 
	count(*) >= 10
order by 
	count(*) desc,
	a."name" 