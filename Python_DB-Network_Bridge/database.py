import MySQLdb

def database_handler():
    "Reads command from commands table and take action depending on command type"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("SELECT * FROM commands")
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        data = c.fetchone()
        if data:
            if( data['ctype'] == 'ping'):
                 print('ping')
                 network.send_ping(data['destination_idb'])
            try:
                c.execute("DELETE FROM commands where idc = {}".format(data['idc']))
            except (MySQLdb.Error, MySQLdb.Warning) as e:
                print(e)
                return None
            db.commit()
        else:
            print("no commands available")
    finally:
        db.close()
    return;

#database_handler();

def add_beacon(x,y,comments, neighbourID):
    "For admin purpose instead of adding beacond in web app - \
    Adds a beacon into all_beacons and then the id in new_beacons table"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("INSERT INTO all_beacons (x, y, comments, neighbourID) \
                       VALUES ({}, {}, '{}', {})".format(x, y, comments, neighbourID)) 
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        idb = c.lastrowid
        db.commit()
        try:
            c.execute("INSERT INTO new_beacons (idnb) \
                       VALUES ({})".format(idb))
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        db.commit()
    finally:
        db.close()
    return; 

#add_beacon(1,2,'c',1)

def add_ping_command(destination_idb):
    "Add command to commands table"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("INSERT INTO commands(ctype, destination_idb) \
                      VALUES('{}', {})".format('ping', destination_idb))
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        db.commit()
    finally:
        db.close()
    return;

def ping_reply(ID, timeStamp):
    "If ID is in new beacons table, will add time stamp and put in active_beacons table\
    and if already in active_beacons table, will update the time stamp"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("SELECT * FROM new_beacons where idnb = {}".format(ID))
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        data = c.fetchone()
        if data:
            print(c.rowcount)
            if(c.rowcount > 0):
                try:
                    c.execute("INSERT INTO active_beacons(idcb) \
                              VALUES({})".format(ID))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                try:
                    c.execute("UPDATE all_beacons SET last_seen = {} where idb = {}".format(timeStamp, ID))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                try:
                    c.execute("DELETE FROM new_beacons where idnb = {}".format(ID))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                db.commit()
        else:
            try:
                c.execute("SELECT * FROM active_beacons where idcb = {}".format(ID))
            except (MySQLdb.Error, MySQLdb.Warning) as e:
                print(e)
                return None
            data = c.fetchone()
            if data:
                if(c.rowcount > 0):
                    try:
                        c.execute("UPDATE all_beacons SET last_seen = {} where idb = {}".format(timeStamp, ID))
                    except (MySQLdb.Error, MySQLdb.Warning) as e:
                        print(e)
                        return None
                    db.commit()
            else:
                print("ID not in new_beacons or active_beacons - the beacon ID was added only to all beacons when created")
    finally:
        db.close()
    return;
#
#check what to do when beacon in new_beacons but already in active_beacons
#
def receive_accident(sourceID, message, timeStamp):
    "Checks if notification is already in latest notifications and if not, \
    checks if it is in history and if the time stamp is less than 24h doesn't add it anymore"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("SELECT * FROM notifications where idb = {}".format(sourceID))
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        data = c.fetchone()
        if data:
            print("already in notifications")
            timeDiff = (timeStamp - data['time']) / 3600
            if(timeDiff < 24):
                 print("can't be real. you got injured too much in past 24 h!")
                 return;
            else:
                 print("God, this dude has bad luck!")
                 try:
                     c.execute("INSERT INTO notifications(idb, ntype, ninfo, time) \
                                  VALUES({}, '{}', '{}', {})".format(sourceID,'accident', message, timeStamp))
                 except (MySQLdb.Error, MySQLdb.Warning) as e:
                     print(e)
                     return None
                 db.commit()
        else:
            try: 
                c.execute("SELECT * FROM history where idb = {}".format(sourceID))
            except (MySQLdb.Error, MySQLdb.Warning) as e:
                print(e)
                return None
            data = c.fetchone()
            if data:
                print("already in history")
                timeDiff = (timeStamp - data['time']) / 3600
                if(timeDiff < 24):
                    print("can't be real. you got injured too much in past 24 h!")
                    return;
                else:
                    print("God, this dude has bad luck!")
                    try:
                        c.execute("INSERT INTO notifications(idb, ntype, ninfo, time) \
                                  VALUES({}, '{}', '{}', {})".format(sourceID,'accident', message, timeStamp))
                    except (MySQLdb.Error, MySQLdb.Warning) as e:
                        print(e)
                        return None
                    db.commit()
            else:
                print("not in history either so add!")
                try:
                    c.execute("INSERT INTO notifications(idb, ntype, ninfo, time) \
                               VALUES({}, '{}', '{}', {})".format(sourceID,'accident', message, timeStamp))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                db.commit()
    finally:
        db.close()
    return;


def cancel_accident(sourceID, timeStamp):
    "Removes accident notification from latest if there and puts it in history with 'cancelled' message type"
    db = MySQLdb.connect(user='root', password='',
                              host='127.0.0.1',
                              database='warden')
    c=db.cursor(MySQLdb.cursors.DictCursor)
    try:
        try:
            c.execute("SELECT * FROM notifications where idb = {}".format(sourceID))
        except (MySQLdb.Error, MySQLdb.Warning) as e:
            print(e)
            return None
        data = c.fetchone()
        if data:
            if(c.rowcount > 0):
                print("removing from list and adding to history")
                try:
                    c.execute("INSERT INTO history(idn, idb, ntype, ninfo, status, time, timeUpdated) \
                                VALUES({}, {}, '{}', '{}', '{}', {}, {})".format(data["idn"], sourceID, 'accident', data["ninfo"], "cancelled", data["time"], timeStamp))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                db.commit()
                try:
                    c.execute("DELETE FROM notifications where idb = {}".format(sourceID))
                except (MySQLdb.Error, MySQLdb.Warning) as e:
                    print(e)
                    return None
                db.commit()
        else:
            print("no current notification with this ID")
    finally:
        db.close()
    return;

#test this

#check why row gets inserted if in history but not in latest without checking time diff - because timeStamp given as parameter is wrong?
#time = 8640100
#for i in range(999900):
    #time += 86401
    #receive_accident(1, "", time)

