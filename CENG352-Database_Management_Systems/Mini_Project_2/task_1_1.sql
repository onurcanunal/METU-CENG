select 
	p.pub_type,
	count(*) as total_number_of_publications
from 
	pub p 
group by
	p.pub_type 
order by 
	count(*) desc 