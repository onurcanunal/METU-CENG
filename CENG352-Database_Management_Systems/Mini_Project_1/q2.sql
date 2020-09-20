select
	ac.airport_code,
	ac.airport_desc,
	count(*) as cancel_count
from
	airport_codes ac,
	flight_reports fr 
where
	ac.airport_code = fr.origin_airport_code
	and fr.is_cancelled = 1
	and fr.cancellation_reason = 'D'
group by
	ac.airport_code
order by
	cancel_count desc,
	ac.airport_code