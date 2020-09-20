select 
	distinct fr.plane_tail_number,
	fr.airline_code as first_owner,
	fr2.airline_code as second_owner
from 
	(select 
		distinct fr.plane_tail_number,
		fr.airline_code,
		fr."year",
		fr."month",
		fr."day"
	from 
		flight_reports fr) as fr,
	(select 
		distinct fr2.plane_tail_number,
		fr2.airline_code,
		fr2."year",
		fr2."month",
		fr2."day"
	from 
		flight_reports fr2) as fr2
where 
	fr.plane_tail_number = fr2.plane_tail_number 
	and fr.airline_code <> fr2.airline_code
	and (fr."year" < fr2."year" 
	or (fr."year" = fr2."year" and fr."month" < fr2."month")
	or (fr."year" = fr2."year" and fr."month" = fr2."month" and fr."day" < fr2."day"))
order by 
	fr.plane_tail_number,
	fr.airline_code,
	fr2.airline_code