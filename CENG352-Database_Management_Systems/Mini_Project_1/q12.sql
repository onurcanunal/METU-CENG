select 
	fr3."year",
	fr3.airline_code,
	boston_flights.number_of_flights as boston_flight_count,
	(boston_flights.number_of_flights * 100.0 / all_flights.number_of_flights) as boston_flight_percentage
from 
	(select 
		fr.airline_code as flights_airline_code,
		fr."year" as flights_year,
		count(*) as number_of_flights
	from
		flight_reports fr
	where 
		fr.dest_city_name like 'Boston%'
		and fr.is_cancelled = 0
	group by 
		fr.airline_code,
		fr."year") as boston_flights,
	(select 
		fr2.airline_code as flights_airline_code,
		fr2."year" as flights_year,
		count(*) as number_of_flights 
	from
		flight_reports fr2
	where 
		fr2.is_cancelled = 0
	group by 
		fr2.airline_code,
		fr2."year") as all_flights,
	flight_reports fr3
where 
	fr3."year" = boston_flights.flights_year
	and fr3."year" = all_flights.flights_year
	and fr3.airline_code = boston_flights.flights_airline_code
	and fr3.airline_code = all_flights.flights_airline_code
group by 
	fr3."year",
	fr3.airline_code,
	boston_flights.number_of_flights,
	all_flights.number_of_flights
having 
	(boston_flights.number_of_flights * 100.0 / all_flights.number_of_flights) > 1
order by 
	fr3."year",
	fr3.airline_code
