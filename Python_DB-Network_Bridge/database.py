import MySQLdb
import network

db = MySQLdb.connect(user='root', passwd='769324',
                              host='127.0.0.1',
                              db='warden')
c=db.cursor(MySQLdb.cursors.DictCursor)


def database_handler():
    "Reads command from commands table and take action depending on command type"
    c.execute("SELECT * FROM commands")
    data = c.fetchone()
    if( data != None and data['ctype'] == 'ping'):
        network.send_ping(data['destination_idb'])
        c.execute("DELETE FROM commands where idc = {}".format(data['idc']))
        db.commit()
    return;


def add_beacon(x,y,comments):
    "Adds a beacon into the all_beacons and then into new_beacons table"
    c.execute("INSERT INTO all_beacons () \
               VALUES ()") 
    idb = c.lastrowid
    db.commit()
    c.execute("INSERT INTO new_beacons (idnb, x, y, comments) \
               VALUES ({}, {}, {}, '{}')".format(idb, x, y, comments))
    db.commit()
    return; 

def add_ping_command(destination):
    "Add command to commands table"
    c.execute("INSERT INTO commands(ctype, destination_idb) \
              VALUES('{}', {})".format('ping', destination))
    db.commit()
    return;

def ping_reply(ID, timeStamp):
    "If ID is in new beacons table, will add time stamp and put in current beacons table\
    and if already in current beacons table, will update the time stamp"
    c.execute("SELECT * FROM new_beacons where idnb = {}".format(ID))
    data = c.fetchone()
    print(c.rowcount)
    if(c.rowcount > 0):
        c.execute("INSERT INTO current_beacons(idcb, x, y, comments, last_seen) \
              VALUES({}, {}, {}, '{}', '{}')".format(ID, data['x'], data['y'], data['comments'], timeStamp))
        c.execute("DELETE FROM new_beacons where idnb = {}".format(ID))
        db.commit()
    else:
        c.execute("UPDATE current_beacons SET last_seen = {} where idcb = {}".format(timeStamp, ID))
        db.commit()
    return;

def receive_accident(sourceID, timestamp, message):
    "Checks if notification is already in latest notifications and if not, \
    checks if it is in history and if the time stamp is less than 24h doesn't add it anymore"
    c.execute("SELECT * FROM notifications where idb = {}".format(sourceID))
    data = c.fetchone()
    if(c.rowcount > 0):
        print("already in notifications")
        return;
    else: 
        c.execute("SELECT * FROM history where idb = {}".format(sourceID))
        data = c.fetchone()
        if(c.rowcount > 0):
            print("already in history")
            timeDiff = (timestamp - data['time']) / 3600
            if(timeDiff < 24):
                print("can't be real. you got injured too much in past 24 h!")
                return;
            else:
                print("God, this dude has bad luck!")
                c.execute("INSERT INTO notifications(idb, ntype, ninfo, time) \
                VALUES({}, '{}', '{}', {})".format(sourceID,'accident', message, timestamp))
                db.commit()
        else:
            print("not in history either so add!")
            c.execute("INSERT INTO notifications(idb, ntype, ninfo, time) \
                VALUES({}, '{}', '{}', {})".format(sourceID,'accident', message, timestamp))
            db.commit()
    return;

def cancel_accident(sourceID, timestamp, message):
    "Removes accident notification from latest if there and puts it in history with 'cancelled' message type"
    c.execute("SELECT * FROM notifications where idb = {}".format(sourceID))
    data = c.fetchone()
    if(c.rowcount > 0):
        print("removing from list and adding to history")
        c.execute("INSERT INTO history(idn, idb, ntype, ninfo, status, time, timeUpdated) \
                VALUES({}, {}, '{}', '{}', '{}', {}, {})".format(data["idn"], sourceID, 'accident', data["ninfo"], "cancelled", data["time"], timestamp))
        db.commit()
        c.execute("DELETE FROM notifications where idb = {}".format(sourceID))
        db.commit()
    return;

 

#db.close()
