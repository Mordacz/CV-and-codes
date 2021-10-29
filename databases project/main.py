import sys
import json
import psycopg2

class Main:

    def read_query(self) :
        query = input()
        data = json.loads(query)
        return data

    def write_answer(self, answer) :
        json.dump(answer, sys.stdout, ensure_ascii=False)
        print()
    
    def init_db(self) :
        init_file = open('init.sql', 'r')
        self.db.execute(init_file.read())
        self.conn.commit()
        ret = dict({'status' : 'OK'})
        return ret

    def flight(self, params) :
        flight_id = params['id']
        A = params['airports']
        n = len(A)
        for i in range(0, n - 1) :
           airport_from, airport_to = A[i]['airport'], A[i + 1]['airport']
           takeoff_time = A[i]['takeoff_time']
           landing_time = A[i + 1]['landing_time']
           SQL = 'SELECT insert_segment(%s, %s, %s, %s, %s);'
           data = (flight_id, airport_from, airport_to, takeoff_time, landing_time)
           self.db.execute(SQL, data)
           self.db.fetchall()
        self.conn.commit()
        ret = dict({'status' : 'OK'})
        return ret

    def list_flights(self, params) :
        flight_id = params['id']
        SQL = 'SELECT * FROM list_flights(%s);'
        data = [flight_id]
        self.db.execute(SQL, data)
        answer = self.db.fetchall()
        ret = dict()
        ret['status'] = 'OK'
        ret['data'] = []
        for row in answer :
            cur_seg = dict({})
            cur_seg['rid'] = row[0]
            cur_seg['from'] = row[1]
            cur_seg['to'] = row[2]
            cur_seg['takeoff_time'] = str(row[3])
            ret['data'].append(cur_seg)
        self.conn.commit()
        return ret
    
    def list_cities(self, params) :
        flight_id = str(params['id'])
        distance_bound  = params['dist']
        SQL = 'SELECT * FROM list_cities(%s, %s);'
        data = (flight_id, distance_bound)
        self.db.execute(SQL, data)
        answer = self.db.fetchall()
        ret = dict()
        ret['status'] = 'OK'
        ret['data'] = []
        for row in answer :
            cur_city = dict({})
            cur_city['name'] = row[0]
            cur_city['prov'] = row[1]
            cur_city['country'] = row[2]
            ret['data'].append(cur_city)
        self.conn.commit()
        return ret

    def list_airport(self, params) :
        airport_id = str(params['iatacode'])
        flights_bound  = params['n']
        SQL = 'SELECT * FROM list_airport(%s, %s);'
        data = (airport_id, flights_bound)
        self.db.execute(SQL, data)
        answer = self.db.fetchall()
        ret = dict()
        ret['status'] = 'OK'
        ret['data'] = []
        for row in answer :
            cur_flight = dict({})
            cur_flight['id'] = row[0]
            ret['data'].append(cur_flight)
        self.conn.commit()
        return ret

    def process_query(self, query) :
        fun = query['function']
        params = query['params']
        if fun == 'flight' :
            return self.flight(params)
        if fun == 'list_flights' :
            return self.list_flights(params)
        if fun == 'list_cities' :
            return self.list_cities(params)
        if fun == 'list_airport' :
            return self.list_airport(params)
        return dict({'status' : 'ERROR', 'description' : 'unknown function \'' + fun + '\''})

    def close(self) :
        self.db.close()
        self.conn.close()
        exit(0)

    def __init__(self) :
        self.conn = psycopg2.connect("dbname=student user=app password=qwerty")
        self.db = self.conn.cursor()

        for opt in sys.argv[1:] :
            if(opt == '--init') :
                self.write_answer(self.init_db())
                self.close()
        while(True) :
            try :
                query = self.read_query()
                answer = self.process_query(query)
                self.write_answer(answer)
            except EOFError :
                self.close()

main = Main()
