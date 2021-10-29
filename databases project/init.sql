--Wysokie loty

--model fizyczny
--physical model

--tabela z segmentami lotu
--table with flight segments
CREATE TABLE flight_seg(
seg_id serial PRIMARY KEY,
flight_id varchar,
from_airport varchar,
to_airport varchar,
takeoff_time timestamp with time zone,
landing_time timestamp with time zone
);

-- będziemy często szukać w airport krotki z konkretną wartością iatacode, więc warto zrobić indeks
--wartości te są unikatowymi krótkimi stringami, stąd hasze wydają się najlepszym wyborem
--we will often ask about records with some fixed value of iatacode in airport, so it's good idea to create index
--as iatacode values are unique short strings, hashes seem to be best choice

DROP INDEX IF EXISTS iatacode_hash_index;
CREATE INDEX iatacode_hash_index ON airport USING hash(iatacode);

-- dodawanie segmentu lotu
-- adding flight segment
CREATE OR REPLACE FUNCTION insert_segment(varchar, varchar, varchar, timestamp with time zone, timestamp with time zone) RETURNS VOID
AS $X$

INSERT INTO flight_seg(flight_id, from_airport, to_airport, takeoff_time, landing_time) VALUES ($1, $2, $3, $4, $5)

$X$ LANGUAGE SQL;

--lista wszystkich segmentów lotów z dodanymi współrzędnymi
--list of all flight segments with coordinates
CREATE OR REPLACE FUNCTION get_flights()
RETURNS TABLE(rid varchar, from_airport varchar, to_airport varchar, takeoff_time timestamp with time zone, from_coords geography, to_coords geography)
AS $X$

SELECT flight_seg.flight_id AS rid, flight_seg.from_airport AS from_airport,  flight_seg.to_airport AS to_airport, flight_seg.takeoff_time AS takeoff_time, ('SRID=4326;POINT(' || a1.longitude::text || ' ' || a1.latitude::text || ')')::geography AS from_coords, ('SRID=4326;POINT(' || a2.longitude::text || ' ' || a2.latitude::text || ')')::geography AS to_coords FROM flight_seg
JOIN airport a1 ON (a1.iatacode = flight_seg.from_airport)
JOIN airport a2 ON (a2.iatacode = flight_seg.to_airport)

$X$ LANGUAGE SQL;

--lista wszystkich segmentów konkretnego lotu z dodanymi współrzędnymi
--list of all segments of fixed flight with coordinates
CREATE OR REPLACE FUNCTION get_single_flight(varchar)
RETURNS TABLE(rid varchar, from_airport varchar, to_airport varchar, takeoff_time timestamp with time zone, from_coords geography, to_coords geography)
AS $X$

SELECT * FROM get_flights()
WHERE rid = $1

$X$ LANGUAGE SQL;

--list_flights
--zakładamy że odległości są zmiennoprzecinkowe, stąd '< eps' zamiast '= 0'
--we assume that distances are floating point numbers, so we use epsilons instead of 0
CREATE OR REPLACE FUNCTION list_flights(varchar)
RETURNS TABLE(rid varchar, from_airport varchar, to_airport varchar, takeoff_time timestamp with time zone)
AS $X$

SELECT DISTINCT all_flights.rid AS rid, all_flights.from_airport AS from_airport, all_flights.to_airport AS to_airport, all_flights.takeoff_time AS takeoff_time FROM
(SELECT * FROM get_flights()) AS all_flights
JOIN get_single_flight($1) current_flight ON 
((ST_Distance(ST_MakeLine(ST_AsText(current_flight.from_coords), ST_AsText(current_flight.to_coords))::geography, ST_MakeLine(ST_AsText(all_flights.from_coords), ST_AsText(all_flights.to_coords))::geography))/1000 < 0.00000001)
WHERE all_flights.rid != current_flight.rid
ORDER BY all_flights.takeoff_time DESC, all_flights.rid ASC

$X$ LANGUAGE SQL;

--list_cities
CREATE OR REPLACE FUNCTION list_cities(varchar, real)
RETURNS TABLE(name varchar, prov varchar, country varchar)
AS $X$

SELECT DISTINCT city.name AS name, city.province AS prov, city.country AS country FROM city
JOIN get_single_flight($1) current_flight ON 
((ST_Distance(ST_MakeLine(ST_AsText(current_flight.from_coords), ST_AsText(current_flight.to_coords))::geography, ('SRID=4326;POINT(' || city.longitude::text || ' ' || city.latitude::text || ')')::geography))/1000 < $2)
ORDER BY city.name ASC

$X$ LANGUAGE SQL;

--list_airport
CREATE OR REPLACE FUNCTION list_airport(varchar, int)
RETURNS TABLE(id varchar)
AS $X$

SELECT flight_id AS id FROM flight_seg
WHERE from_airport = $1
ORDER BY takeoff_time DESC, flight_id ASC
LIMIT $2

$X$ LANGUAGE SQL;
