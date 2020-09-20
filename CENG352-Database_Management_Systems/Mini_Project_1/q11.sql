select 
	ac.airline_name,
	fr3."year",
	z.total_num_flights,
	count(*) as cancelled_flights
from
	(select 
		fr2.airline_code as airline_code,
		fr2."year" as "year",
		count(*) as total_num_flights
	from	
		(select
			x.airline_code as airline_code 
		from
			(select 
				fr.airline_code as airline_code,
				count(*) as flight_counts
			from
				flight_reports fr 
			group by
				fr.airline_code,
				fr."day",
				fr."month",
				fr."year" 
			order by
				fr.airline_code) as x
		group by 
			x.airline_code
		having 
			avg(x.flight_counts) > 2000) as y,
		flight_reports fr2
	where 
		y.airline_code = fr2.airline_code
	group by 
		fr2.airline_code,
		fr2."year") as z,
	flight_reports fr3,
	airline_codes ac
where 
	fr3.airline_code = z.airline_code
	and fr3."year" = z."year"
	and fr3.is_cancelled = 1
	and ac.airline_code = fr3.airline_code
group by 
	fr3.airline_code,
	ac.airline_name,
	fr3."year",
	z.total_num_flights
order by 
	ac.airline_name 