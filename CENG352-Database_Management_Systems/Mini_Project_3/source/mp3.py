from customer import Customer

import psycopg2

from config import read_config
from messages import *

POSTGRESQL_CONFIG_FILE_NAME = "database.cfg"

"""
    Connects to PostgreSQL database and returns connection object.
"""


def connect_to_db():
    db_conn_params = read_config(filename=POSTGRESQL_CONFIG_FILE_NAME, section="postgresql")
    conn = psycopg2.connect(**db_conn_params)
    conn.autocommit = False
    return conn


"""
    Splits given command string by spaces and trims each token.
    Returns token list.
"""


def tokenize_command(command):
    tokens = command.split(" ")
    return [t.strip() for t in tokens]


"""
    Prints list of available commands of the software.
"""


def help():
    # prints the choices for commands and parameters
    print("\n*** Please enter one of the following commands ***")
    print("> help")
    print("> sign_up <email> <password> <first_name> <last_name> <plan_id>")
    print("> sign_in <email> <password>")
    print("> sign_out")
    print("> show_plans")
    print("> show_subscription")
    print("> subscribe <plan_id>")
    print("> watched_movies <movie_id_1> <movie_id_2> <movie_id_3> ... <movie_id_n>")
    print("> search_for_movies <keyword_1> <keyword_2> <keyword_3> ... <keyword_n>")
    print("> suggest_movies")
    print("> quit")


"""
    Saves customer with given details.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
"""


def sign_up(conn, email, password, first_name, last_name, plan_id):
    
    cur = conn.cursor()

    try:
        cur.execute("INSERT INTO Customer (email, password, first_name, last_name, session_count, plan_id) VALUES (%s, %s, %s, %s, %s, %s)", 
            (email, password, first_name, last_name, 0, plan_id))
        conn.commit()
        cur.close()
        return True, CMD_EXECUTION_SUCCESS
    except: 
        conn.rollback()
        cur.close()
        return False, CMD_EXECUTION_FAILED

"""
    Retrieves customer information if email and password is correct and customer's session_count < max_parallel_sessions.
    - Return type is a tuple, 1st element is a customer object and 2nd element is the response message from messages.py.
    - If email or password is wrong, return tuple (None, USER_SIGNIN_FAILED).
    - If session_count < max_parallel_sessions, commit changes (increment session_count) and return tuple (customer, CMD_EXECUTION_SUCCESS).
    - If session_count >= max_parallel_sessions, return tuple (None, USER_ALL_SESSIONS_ARE_USED).
    - If any exception occurs; rollback, do nothing on the database and return tuple (None, USER_SIGNIN_FAILED).
"""


def sign_in(conn, email, password):

    cur = conn.cursor()

    try:
        cur.execute("SELECT * FROM Customer WHERE email = %s AND password = %s",
            (email, password))
        row = cur.fetchone()
        if row is None:
            cur.close()
            return None, USER_SIGNIN_FAILED
        cur.execute("SELECT max_parallel_sessions FROM Plan WHERE plan_id = %s", (row[-1],))
        row2 = cur.fetchone()
        if row[5] >= row2[0]:
            cur.close()
            return None, USER_ALL_SESSIONS_ARE_USED
        elif row[5] < row2[0]:
            cur.execute("UPDATE Customer SET session_count = session_count + 1 WHERE customer_id = %s", (row[0],))
            conn.commit()
            customer = Customer(row[0], row[1], row[3], row[4], row[5], row[6])
            cur.close()
            return customer, CMD_EXECUTION_SUCCESS
    except:
        conn.rollback()
        cur.close()
        return None, USER_SIGNIN_FAILED


"""
    Signs out from given customer's account.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - Decrement session_count of the customer in the database.
    - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
"""


def sign_out(conn, customer):
    
    cur = conn.cursor()

    try:
        cur.execute("SELECT * FROM Customer WHERE customer_id = %s",
            (customer.customer_id,))
        row = cur.fetchone()
        if row is None:
            cur.close()
            return False, CMD_EXECUTION_FAILED
        elif row[5] <= 0:
            cur.close()
            return False, CMD_EXECUTION_FAILED
        else:
            cur.execute("UPDATE Customer SET session_count = session_count - 1 WHERE customer_id = %s",
                (customer.customer_id,))
            conn.commit()
            cur.close()
            return True, CMD_EXECUTION_SUCCESS
    except:
        conn.rollback()
        cur.close()
        return False, CMD_EXECUTION_FAILED


"""
    Quits from program.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - Remember to sign authenticated user out first.
    - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
"""


def quit(conn, customer):

    try:
        if customer is not None:
            sign_out(conn, customer)
        return True, CMD_EXECUTION_SUCCESS
    except:
        return False, CMD_EXECUTION_FAILED


"""
    Retrieves all available plans and prints them.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - If the operation is successful; print available plans and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

    Output should be like:
    #|Name|Resolution|Max Sessions|Monthly Fee
    1|Basic|720P|2|30
    2|Advanced|1080P|4|50
    3|Premium|4K|10|90
"""


def show_plans(conn):

    cur = conn.cursor()

    try:
        print("#|Name|Resolution|Max Sessions|Monthly Fee")
        cur.execute("SELECT * FROM Plan")
        rows = cur.fetchall()
        for row in rows:
            print(str(row[0]) + "|" + str(row[1]) + "|" + str(row[2]) + "|" + str(row[3]) + "|" + str(row[4]))
        cur.close()
        return True, CMD_EXECUTION_SUCCESS
    except:
        cur.close()
        return False, CMD_EXECUTION_FAILED


"""
    Retrieves authenticated user's plan and prints it. 
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - If the operation is successful; print the authenticated customer's plan and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

    Output should be like:
    #|Name|Resolution|Max Sessions|Monthly Fee
    1|Basic|720P|2|30
"""


def show_subscription(conn, customer):
    
    cur = conn.cursor()

    try:
        print("#|Name|Resolution|Max Sessions|Monthly Fee")
        cur.execute("SELECT plan_id FROM Customer WHERE customer_id = %s", (customer.customer_id,))
        row = cur.fetchone()
        cur.execute("SELECT * FROM Plan WHERE plan_id = %s", (row[0],))
        row2 = cur.fetchone()
        print(str(row2[0]) + "|" + str(row2[1]) + "|" + str(row2[2]) + "|" + str(row2[3]) + "|" + str(row2[4]))
        cur.close()
        return True, CMD_EXECUTION_SUCCESS
    except:
        cur.close()
        return False, CMD_EXECUTION_FAILED

"""
    Insert customer-movie relationships to Watched table if not exists in Watched table.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - If a customer-movie relationship already exists, do nothing on the database and return (True, CMD_EXECUTION_SUCCESS).
    - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any one of the movie ids is incorrect; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
"""


def watched_movies(conn, customer, movie_ids):
    
    cur = conn.cursor()

    try:
        for movie_id in movie_ids:
            cur.execute("SELECT * FROM Movies WHERE movie_id = %s", (movie_id,))
            row = cur.fetchone()

            if row is None:
                conn.rollback()
                cur.close()
                return False, CMD_EXECUTION_FAILED
            else:
                cur.execute("SELECT * FROM Watched WHERE customer_id = %s AND movie_id = %s",
                    (customer.customer_id, movie_id,))
                row2 = cur.fetchone()
                if row2 is None:
                    cur.execute("INSERT INTO Watched (customer_id, movie_id) VALUES (%s, %s)",
                        (customer.customer_id, movie_id,))
        conn.commit()
        cur.close()
        return True, CMD_EXECUTION_SUCCESS

    except:
        conn.rollback()
        cur.close()
        return False, CMD_EXECUTION_FAILED


"""
    Subscribe authenticated customer to new plan.
    - Return type is a tuple, 1st element is a customer object and 2nd element is the response message from messages.py.
    - If target plan does not exist on the database, return tuple (None, SUBSCRIBE_PLAN_NOT_FOUND).
    - If the new plan's max_parallel_sessions < current plan's max_parallel_sessions, return tuple (None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE).
    - If the operation is successful, commit changes and return tuple (customer, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; rollback, do nothing on the database and return tuple (None, CMD_EXECUTION_FAILED).
"""


def subscribe(conn, customer, plan_id):
    
    cur = conn.cursor()

    try:
        cur.execute("SELECT max_parallel_sessions FROM Plan WHERE plan_id = %s", (plan_id,))
        row = cur.fetchone()
        if row is None:
            cur.close()
            return None, SUBSCRIBE_PLAN_NOT_FOUND
        cur.execute("SELECT max_parallel_sessions FROM Plan WHERE plan_id = %s", (customer.plan_id,))
        row2 = cur.fetchone()
        if row[0] < row2[0]:
            cur.close()
            return None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE
        else:
            cur.execute("UPDATE Customer SET plan_id = %s WHERE customer_id = %s", 
                (plan_id, customer.customer_id,))
            conn.commit()
            cur.close()
            return customer, CMD_EXECUTION_SUCCESS
    except:
        cur.close()
        conn.rollback()
        return None, CMD_EXECUTION_FAILED

"""
    Searches for movies with given search_text.
    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
    - Print all movies whose titles contain given search_text IN CASE-INSENSITIVE MANNER.
    - If the operation is successful; print movies found and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

    Output should be like:
    Id|Title|Year|Rating|Votes|Watched
    "tt0147505"|"Sinbad: The Battle of the Dark Knights"|1998|2.2|149|0
    "tt0468569"|"The Dark Knight"|2008|9.0|2021237|1
    "tt1345836"|"The Dark Knight Rises"|2012|8.4|1362116|0
    "tt3153806"|"Masterpiece: Frank Millers The Dark Knight Returns"|2013|7.8|28|0
    "tt4430982"|"Batman: The Dark Knight Beyond"|0|0.0|0|0
    "tt4494606"|"The Dark Knight: Not So Serious"|2009|0.0|0|0
    "tt4498364"|"The Dark Knight: Knightfall - Part One"|2014|0.0|0|0
    "tt4504426"|"The Dark Knight: Knightfall - Part Two"|2014|0.0|0|0
    "tt4504908"|"The Dark Knight: Knightfall - Part Three"|2014|0.0|0|0
    "tt4653714"|"The Dark Knight Falls"|2015|5.4|8|0
    "tt6274696"|"The Dark Knight Returns: An Epic Fan Film"|2016|6.7|38|0
"""


def search_for_movies(conn, customer, search_text):
    
    cur = conn.cursor()

    try:
        search_text = "%" + search_text + "%"
        cur.execute("SELECT * FROM Movies WHERE title ILIKE %s ORDER BY movie_id", (search_text,))
        rows = cur.fetchall()

        print("Id|Title|Year|Rating|Votes|Watched")
        for row in rows:
            watched = 0
            cur.execute("SELECT * FROM Watched WHERE customer_id = %s AND movie_id = %s", 
                (customer.customer_id, row[0],))
            row2 = cur.fetchone()
            if row2 is not None:
                watched = 1
            print(str(row[0]) + "|" + str(row[1]) + "|" + str(row[2]) + "|" + str(row[3])
                + "|" + str(row[4]) + "|" + str(watched))
        cur.close()
        return True, CMD_EXECUTION_SUCCESS 
    except:
        cur.close()
        return False, CMD_EXECUTION_FAILED


"""
    Suggests combination of these movies:
        1- Find customer's genres. For each genre, find movies with most number of votes among the movies that the customer didn't watch.

        2- Find top 10 movies with most number of votes and highest rating, such that these movies are released 
           after 2010 ( [2010, today) ) and the customer didn't watch these movies.
           (descending order for votes, descending order for rating)

        3- Find top 10 movies with votes higher than the average number of votes of movies that the customer watched.
           Disregard the movies that the customer didn't watch.
           (descending order for votes)

    - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.    
    - Output format and return format are same with search_for_movies.
    - Order these movies by their movie id, in ascending order at the end.
    - If the operation is successful; print movies suggested and return tuple (True, CMD_EXECUTION_SUCCESS).
    - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).
"""


def suggest_movies(conn, customer):
    
    cur = conn.cursor()

    try:
        cur.execute("SELECT M.movie_id, M.title, M.movie_year, M.rating, M.votes from (SELECT G.genre_name, MAX(M.votes) as max_vote_count FROM (SELECT Movies.movie_id as movie_id FROM Movies EXCEPT SELECT Movies.movie_id FROM Movies, Watched WHERE Movies.movie_id = Watched.movie_id AND Watched.customer_id = %s) as Temp, genres G, movies M,(select g.genre_name from watched w, genres g where w.customer_id = %s and w.movie_id = g.movie_id) as customer_genres WHERE Temp.movie_id = G.movie_id AND G.movie_id = M.movie_id and customer_genres.genre_name = G.genre_name GROUP BY G.genre_name) as max_votes,Movies M,Genres G where M.movie_id = G.movie_id and G.genre_name = max_votes.genre_name and M.votes = max_votes.max_vote_count",
            (customer.customer_id, customer.customer_id,))
        movies_s1 = cur.fetchall()

        cur.execute("SELECT movie_id, title, movie_year, rating, votes FROM Movies WHERE movie_year >= 2010 EXCEPT SELECT M.movie_id, M.title, M.movie_year, M.rating, M.votes FROM Movies M, Watched W WHERE M.movie_id = W.movie_id AND W.customer_id = %s ORDER BY votes DESC, rating DESC LIMIT 10",
            (customer.customer_id,))
        movies_s2 = cur.fetchall()

        cur.execute("SELECT movie_id, title, movie_year, rating, votes FROM (SELECT AVG(M.votes) as avgRating FROM Movies M, Watched W WHERE M.movie_id = W.movie_id AND W.customer_id = %s) as Temp, Movies WHERE Movies.votes > Temp.avgRating EXCEPT SELECT M.movie_id, M.title, M.movie_year, M.rating, M.votes FROM Movies M, Watched W WHERE M.movie_id = W.movie_id AND W.customer_id = %s ORDER BY votes DESC LIMIT 10", 
            (customer.customer_id, customer.customer_id,))
        movies_s3 = cur.fetchall()

        all_movies = list(set(movies_s1 + movies_s2 + movies_s3))
        all_movies.sort(key=lambda tup: tup[0])
        print("Id|Title|Year|Rating|Votes")
        for movie in all_movies:
            print(str(movie[0]) + "|" + str(movie[1]) + "|" + str(movie[2]) + "|" + str(movie[3]) + "|" + str(movie[4]))
        cur.close()
        return True, CMD_EXECUTION_SUCCESS
    except:
        cur.close()
        return False, CMD_EXECUTION_FAILED
