select 
	ac.airline_name 
from
	(
		(
			(select 
				fr.airline_code as airline_code
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Boston, MA'
				and (fr."year" = 2018 or fr."year" = 2019)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'New York, NY'
				and (fr."year" = 2018 or fr."year" = 2019)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Portland, ME'
				and (fr."year" = 2018 or fr."year" = 2019)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Washington, DC'
				and (fr."year" = 2018 or fr."year" = 2019)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Philadelphia, PA'
				and (fr."year" = 2018 or fr."year" = 2019)
			)
		)
		except 
		(
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Boston, MA'
				and (fr."year" = 2016 or fr."year" = 2017)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'New York, NY'
				and (fr."year" = 2016 or fr."year" = 2017)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Portland, ME'
				and (fr."year" = 2016 or fr."year" = 2017)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Washington, DC'
				and (fr."year" = 2016 or fr."year" = 2017)
			)
			intersect
			(select 
				fr.airline_code 
			from 
				flight_reports fr 
			where 
				fr.is_cancelled = 0
				and fr.dest_city_name = 'Philadelphia, PA'
				and (fr."year" = 2016 or fr."year" = 2017)
			)
		)
	) as x,
	airline_codes ac
where 
	x.airline_code = ac.airline_code 
order by 
	ac.airline_name 