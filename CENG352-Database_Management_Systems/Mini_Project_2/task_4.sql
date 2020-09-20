create or replace function proc_active() returns trigger as $active_func$
	begin 
		if(select
			count(*)
		  from 
		  	"publication" p
		  where
		  	new.pub_id = p.pub_id
		  	and p."year" >= '2018'
			and p."year" <= '2020') > 0 
		then
		  insert into
		  	activeauthors(name)
		  select
		  	a."name" 
		  from 
		  	author a
		  where
		  	a.author_id = new.author_id;
		end if;
		return null;
	end;
$active_func$ language plpgsql;

create trigger active_func
after insert on authored
	for each row execute procedure proc_active();
	