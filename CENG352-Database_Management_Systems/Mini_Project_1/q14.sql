select 
	fr3."year",
	w.weekday_name,
	c.reason_desc as reason,
	count(*) as number_of_cancellations
from
	(select 
		fr2."year" as max_year,
		fr2.weekday_id as max_weekday_id,
		max(daily_cancellation_table.number_of_cancellation) as max_number_of_cancellation
	from 
		(select 
			fr."year" as "year",
			fr.weekday_id as weekday_id,
			fr.cancellation_reason as cancellation_reason,
			count(*) as number_of_cancellation
		from 
			flight_reports fr 
		where 
			fr.is_cancelled = 1
		group by 
			fr."year",
			fr.weekday_id,
			fr.cancellation_reason) as daily_cancellation_table,
		flight_reports fr2
	where 
		fr2.is_cancelled = 1
		and fr2."year" = daily_cancellation_table."year"
		and fr2.weekday_id = daily_cancellation_table.weekday_id
		and fr2.cancellation_reason = daily_cancellation_table.cancellation_reason
	group by 
		fr2."year",
		fr2.weekday_id) as daily_max,
	flight_reports fr3,
	weekdays w,
	cancellation_reasons c
where 
	fr3.is_cancelled = 1
	and fr3."year" = daily_max.max_year
	and fr3.weekday_id = daily_max.max_weekday_id
	and fr3.weekday_id = w.weekday_id 
	and c.reason_code = fr3.cancellation_reason 
group by 
	fr3."year",
	w.weekday_id,
	w.weekday_name,
	c.reason_code,
	c.reason_desc
having 
	count(*) = max(daily_max.max_number_of_cancellation)
order by 
	fr3."year",
	w.weekday_id