insert into 
	author(name)
select 
	distinct f.field_value 
from 
	field f
where
	f.field_name = 'author'
	
insert into 
	"publication"(pub_key, title, "year")
select
	distinct f.pub_key,
	f.field_value,
	cast(f2.field_value as int) 
from 
	field f,
	field f2 
where
	f.pub_key = f2.pub_key
	and f.field_name = 'title'
	and f2.field_name = 'year'
	
insert into 
	article(pub_id, journal, "month", volume, "number")
select
	p2.pub_id,
	f.field_value,
	f2.field_value,
	f3.field_value,
	f4.field_value 
from 
	pub p
inner join "publication" as p2 on p.pub_key = p2.pub_key and p.pub_type = 'article'
left join field as f on p.pub_key = f.pub_key and f.field_name = 'journal'
left join field as f2 on p.pub_key = f2.pub_key and f2.field_name = 'month'
left join field as f3 on p.pub_key = f3.pub_key and f3.field_name = 'volume'
left join field as f4 on p.pub_key = f4.pub_key and f4.field_name = 'number'

insert into 
	book(pub_id, publisher, isbn)
select
	p2.pub_id,
	f.field_value,
	max(f2.field_value)
from
	pub p
inner join "publication" as p2 on p.pub_key = p2.pub_key and p.pub_type = 'book'
left join field as f on p.pub_key = f.pub_key and f.field_name = 'publisher'
left join field as f2 on p.pub_key = f2.pub_key and f2.field_name = 'isbn'
group by 
	p2.pub_id,
	f.field_value 
	
insert into 
	incollection(pub_id, book_title, publisher, isbn)
select
	p2.pub_id,
	f.field_value,
	f2.field_value,
	max(f3.field_value)
from 
	pub p
inner join "publication" as p2 on p.pub_key = p2.pub_key and p.pub_type = 'incollection'
left join field as f on p.pub_key = f.pub_key and f.field_name = 'title'
left join field as f2 on p.pub_key = f2.pub_key and f2.field_name = 'publisher'
left join field as f3 on p.pub_key = f3.pub_key and f3.field_name = 'isbn'
group by 
	p2.pub_id,
	f.field_value,
	f2.field_value 
	
insert into 
	inproceedings(pub_id, book_title, editor)
select
	p2.pub_id,
	f.field_value,
	f2.field_value
from 
	pub p
inner join "publication" as p2 on p.pub_key = p2.pub_key and p.pub_type = 'inproceedings'
left join field as f on p.pub_key = f.pub_key and f.field_name = 'title'
left join field as f2 on p.pub_key = f2.pub_key and f2.field_name = 'editor'

insert into 
	authored(author_id, pub_id)
select
	distinct a.author_id,
	p.pub_id
from 
	field f
inner join author as a on f.field_name = 'author' and f.field_value = a."name"
inner join "publication" as p on f.pub_key = p.pub_key

