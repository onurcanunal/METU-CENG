select 
	f2.field_name
from 
	pub p2,
	field f2
where
	p2.pub_key = f2.pub_key 
group by 
	f2.field_name
having 
	count(distinct p2.pub_type) = (select 
										count(distinct p3.pub_type)
								   from 
										pub p3)
order by 
	f2.field_name 
