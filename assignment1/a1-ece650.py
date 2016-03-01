import sys
import re
lineCount = 1
pointCount = 1
streets = []
newpoint={}
pointID = {}

def isExistV(v,m):
    for each in v:
        if(m==each):
            return True
    return False

def isExistE(e,t):
    for each in e:
        if(t==each):
            return True
    return False

def print_graph():
    # print 'in print_graph'
    v = []
    e = []
    # print newpoint.keys()
    for eachkey in newpoint.keys():
        originalPoint = [eachkey.split(',')[0],eachkey.split(',')[1]]
        # print 'originalPoint-->',originalPoint
        printpoint = newpoint.get(eachkey)
        # print 'printpoint--->',printpoint
        p1id = pointID.get(eachkey)
        if(isExistV(v,originalPoint)==False):
            v.append(originalPoint)
        # print 'in eachkey-->',printpoint
        # print 'v',v
        for each in printpoint:
            # print 'each--->',each
            streetname1 = each.split('-')[0]
            linename1 = each.split('-')[1]
            street1 = get_streetByName(streetname1)
            # print 'in pr',street1
            # print 'in each printpoint-->',street1[0]
            for j in range(1,len(street1)):
                if(street1[j][0]==linename1):
                    printline = street1[j]
                    # print 'in printline->',printline
                    if(originalPoint==printline[1]):
                        if(j>1):
                            if(isExistV(v,street1[j-1][-2])==False):
                                v.append(street1[j-1][-2])
                            p3id = pointID.get(street1[j-1][-2][0]+','+street1[j-1][-2][1])
                            ee2 = [p3id,p1id]
                            if(p3id!=p1id):
                                if(isExistE(e,ee2)==False):
                                    e.append(ee2)
                        if(isExistV(v,printline[3])==False):
                            v.append(printline[3])
                        p6id = pointID.get(printline[3][0]+','+printline[3][1])
                        ee3 = [p1id,p6id]
                        if(isExistE(e,ee3)==False):
                            e.append(ee3)
                        break
                    if(originalPoint==printline[-1]):
                        # print 'in end -->',originalPoint
                        if(j<len(street1)-1):
                            # print 'in small one...'
                            if(isExistV(v,street1[j+1][2])==False):
                                v.append(street1[j+1][2])
                            p4id = pointID.get(street1[j+1][2][0]+','+street1[j+1][2][1])
                            ee4 = [p1id,p4id]
                            if(p1id!=p4id):
                                if(isExistE(e,ee4)==False):
                                    e.append(ee4)
                        if(isExistV(v,printline[-3])==False):
                            v.append(printline[-3])
                        p5id = pointID.get(printline[-3][0]+','+printline[-3][1])
                        ee5 = [p5id,p1id]
                        if(isExistE(e,ee5)==False):
                            e.append(ee5)
                        # print 'here...'
                        break
                    for k in range(2,len(printline)-1):
                        if(printline[k]==originalPoint):
                            if(isExistV(v,printline[k-1])==False):
                                v.append(printline[k-1])
                            if(isExistV(v,printline[k+1])==False):
                                v.append(printline[k+1])
                            p2id = pointID.get(printline[k-1][0]+','+printline[k-1][1])
                            p5id = pointID.get(printline[k+1][0]+','+printline[k+1][1])
                            ee = [p2id,p1id]
                            ee1 = [p1id,p5id]
                            if(isExistE(e,ee)==False):
                                e.append(ee)
                            if(isExistE(e,ee1)==False):
                                e.append(ee1)

    count = 1
    newpID = {}
    for each in v:
         newpID[pointID.get(each[0]+','+each[1])] = count
         count = count + 1

    count = 1
    print 'V = {'
    for each in v:
        aaa = ("%.2f" % float(each[0]))
        bbb = ("%.2f" % float(each[1]))
        print count , ": (" ,aaa,",",bbb,"),"
        count = count + 1
    print '}\n E = {'
    for each1 in e:
        print '<',newpID.get(each1[0]),',',newpID.get(each1[1]),'>,'
    print '}'

def del_point(streetname,linename,point):
    street = get_streetByName(streetname)
    for each in street:
        if(each[0]==linename):
            each.remove(point)
            return

def isPointExistSameLine(pv):
    st1 = pv[0].split('-')[0]
    for i in range(1,len(pv)) :
        streetname1 = pv[i].split('-')[0]
        if(st1!=streetname1):
            return False
    return True

def isPointExistSameLineOther(pv,judgename):
    for i in range(0,len(pv)) :
        streetname1 = pv[i].split('-')[0]
        if(judgename==streetname1):
            return True
    return False

def isVertex(street,line,p):
    ''' judge if p is a vertext '''
    for i in range(1,len(street)):
        if(street[i][0]==line):
            m = street[i]
            if(p==m[1] or p==m[-1]):
                return True
    return False

def remove_line(line,name):
    for i in range(2,len(line)-1):
        pointValue = newpoint.get(line[i][0]+','+line[i][1])
        pointValue.remove(name+'-'+line[0])
        if(len(pointValue)==1):
            streetname = pointValue[0].split('-')[0]
            linename = pointValue[0].split('-')[1]
            # if(isVertex(get_streetByName(streetname),linename,line[i])==False):
            del_point(streetname,linename,line[i])
            newpoint.pop(line[i][0]+','+line[i][1])
            # pointID.pop(line[i][0]+','+line[i][1])
        elif(len(pointValue)>=2):
            if(isPointExistSameLine(pointValue)):
                for each in pointValue:
                    streetname2 = each.split('-')[0]
                    linename2 = each.split('-')[1]
                    del_point(streetname2,linename2,line[i])
                newpoint.pop(line[i][0]+','+line[i][1])
                pointID.pop(line[i][0]+','+line[i][1])


def remove_street(name):
    if(isExistInStreets(name)==False):
        errormessage = 'Error:"r" specified for a street "'+name+'" that does not exist!\n'
        sys.stderr.write(errormessage)
        return
    street = get_streetByName(name)
    # print 'in remove--->',street[0]
    if(street==None):
        errormessage = 'Error:"r" specified for a street that does not exist!\n'
        sys.stderr.write(errormessage)
        return
    for i in range(1,len(street)):
        if(len(street[i])>3):
            remove_line(street[i],street[0])
    streets.remove(street)

def get_streetByName(name):
    for each in streets:
        if(each[0]==name):
            # print each[0]
            return each
    return None

def isVertix(line1,line2,m):
    for i in range(1,len(line1)):
        if(m==line1[i]):
            return True
    for j in range(1,len(line2)):
        if(m==line1[j]):
            return True
    return False

def isExistValues(values,m):
    for each in values:
        if(each==m):
            return True
    return False

def addToLine(line,m):
#     print 'line->',line[1],'m->',m
#     print 'in addToLine--->',line[1]==m
    if(m==line[1]):
        if(len(line)>=4 and line[2]==m):
            return
        line.insert(1,m)
        return
    if(m==line[-1]):
        if(len(line)>=4 and line[-2]==m):
            return
        line.insert(-1,m)
        return
    for i in range(1,len(line)-1):
        x1 = float(line[i][0])
        y1 = float(line[i][1])
        x2 = float(line[i+1][0])
        y2 = float(line[i+1][1])
        x = float(m[0])
        y = float(m[1])
        if((x1==x2 and (y-y1)*(y-y2)<0) or (y1==y2 and (x-x1)*(x-x2)<0) or (x1!=x2 and (x-x1)*(x-x2)<0) or (y1!=y2 and (y-y1)*(y-y2)<0 )):
            line.insert(i+1,m)
            return

def get_intersectionInTwoLine(line1,line2):
    m = []
    t = set([])
    x1 = float(line1[1][0])
    y1 = float(line1[1][1])
    x2 = float(line1[-1][0])
    y2 = float(line1[-1][1])
    x3 = float(line2[1][0])
    y3 = float(line2[1][1])
    x4 = float(line2[-1][0])
    y4 = float(line2[-1][1])
    if((x3-x4)*(y1-y2)!=(x1 - x2) * (y3 - y4)):
        x = ((x1 - x2)*(x3*y4 - x4*y3) - (x3 - x4) * (x1 * y2 - x2 * y1))/((x3 - x4) * (y1 - y2) - (x1 - x2) * (y3 - y4));
        y = ((y1 - y2) * (x3 * y4 - x4 * y3) - (x1 * y2 - x2 * y1) * (y3 - y4))/ ((y1 - y2) * (x3 - x4) - (x1 - x2) * (y3 - y4));
        if((x-x1)*(x-x2)<=0and(y-y1)*(y-y2)<=0and(x-x3)*(x-x4)<=0and(y-y3)*(y-y4)<=0):
            return [str(x),str(y)]
    else :
        t.add((x1,y1))
        t.add((x2,y2))
        if(len(t) == 2):
            t.add((x3,y3))
            if(len(t) == 2):
                t.add((x4,y4))
                if(len(t)==2):
                    return None
                return [str(x3),str(y3)]
            else :
                t.add((x4,y4))
                if(len(t) == 3):
                    return [str(x4),str(y4)]
                else:
                    return None
        else:
            return None


def get_intersectionByTwoLine(line1,line2,name1,name2):
    # print 'in get_intersectionByTwoLine--',line1[0],'<-->',line2[0]
    global pointCount
    m = get_intersectionInTwoLine(line1,line2)
    if(m != None):
        x = m[0]
        y = m[1]
        t = newpoint.get(str(x)+','+str(y))
        if(t!=None):
            ''' already exist! '''
            if(isExistValues(t,name1+'-'+line1[0])==False):
                t.append(name1+'-'+line1[0])
            if(isPointExistSameLineOther(t,name2)):
                ''' there is a condition that intersection by themselves'''
                if(isExistValues(t,name2+'-'+line2[0])==False):
                    t.append(name2+'-'+line2[0])
                addToLine(line2,m)
        else:
            ''' the first time to get this point '''
            t = [name1+'-'+line1[0],name2+'-'+line2[0]]
            if(pointID.get(str(x)+','+str(y))==None):
                pointID[str(x)+','+str(y)] = pointCount
                pointCount = pointCount+1
            addToLine(line2,m)
        # print 'in add--->',m
        addToLine(line1,m)
        newpoint[str(x)+','+str(y)] = t

def get_intersectionPoint(street):
    for each in streets:
        for i in range(1,len(street)):
            for j in range(1,len(each)):
                get_intersectionByTwoLine(street[i],each[j],street[0],each[0])

def judgePoint(p1,p2,p3):
    x1 = float(p1[0])
    y1 = float(p1[1])
    x2 = float(p2[0])
    y2 = float(p2[1])
    x  = float(p3[0])
    y  = float(p3[1])
    if((x1==x2 and x==x2) or ((y2-y1)*(x-x2)==(y-y2)*(x2-x1))):
        errormessage = 'Error:the input three points('+p1[0]+','+p1[1]+'),('+p2[0]+','+p2[1]+'),('+p3[0]+','+p3[1]+') are at the same line!\n'
        sys.stderr.write(errormessage)
        return False
    elif((x-x1)*(x-x2)<0 and (y-y1)*(y-y2)<0):
        errormessage = 'Error:the input three points('+p1[0]+','+p1[1]+'),('+p2[0]+','+p2[1]+'),('+p3[0]+','+p3[1]+') are at the same line and overlap!\n'
        sys.stderr.write(errormessage)
        return False
    else:
        return True


def judge(street):
    s = street
    for i in range(1,len(s)-1):
        if(judgePoint(s[i][1],s[i][-1],s[i+1][-1])==False):
            return False
    return True

def judgeTwoLines(line1,line2):
    m = []
    t = set([])
    x1 = float(line1[1][0])
    y1 = float(line1[1][1])
    x2 = float(line1[-1][0])
    y2 = float(line1[-1][1])
    x3 = float(line2[1][0])
    y3 = float(line2[1][1])
    x4 = float(line2[-1][0])
    y4 = float(line2[-1][1])
    # print line1,line2
    # print 'here'
    if((x3-x4)*(y1-y2)!=(x1 - x2) * (y3 - y4)):
        x = ((x1 - x2)*(x3*y4 - x4*y3) - (x3 - x4) * (x1 * y2 - x2 * y1))/((x3 - x4) * (y1 - y2) - (x1 - x2) * (y3 - y4));
        y = ((y1 - y2) * (x3 * y4 - x4 * y3) - (x1 * y2 - x2 * y1) * (y3 - y4))/ ((y1 - y2) * (x3 - x4) - (x1 - x2) * (y3 - y4));
        if((x-x1)*(x-x2)<=0and(y-y1)*(y-y2)<=0and(x-x3)*(x-x4)<=0and(y-y3)*(y-y4)<=0):
            return True
    else :
        # print 'there'
        t.add((x1,y1))
        t.add((x2,y2))
        # print len(t),t
        if(len(t) == 2):
            # print 'kkk'
            t.add((x3,y3))
            # print len(t)
            if(len(t) == 2):
                # print 'there'
                t.add((x4,y4))
                # print len(t)
                if(len(t) == 2):
                    errorMsg = "Error:the line "+str(line1)+" and line"+str(line2)+" overlap !\n"
                    sys.stderr.write(errorMsg)
                    return False
                elif((x4-x1)*(x4-x2)<=0 or (y4-y1)*(y4-y2)<=0) :
                    errorMsg = "Error:the line "+str(line1)+" and line2"+str(line2)+" overlap !\n"
                    sys.stderr.write(errorMsg)
                    return False
                else :
                    return True
            else :
                t.add((x4,y4))
                # print len(t),t,x4,y4,y1,y2
                if(len(t) == 3):
                    # print 'here'
                    if((x4-x1)*(x4-x2)<=0 or (y4-y1)*(y4-y2)<=0) :
                        # print (x4-x1)*(x4-x2) , (y4-y1) , (y4-y2)
                        errorMsg = "Error:the line "+str(line1)+" and line2"+str(line2)+" overlap !\n"
                        sys.stderr.write(errorMsg)
                        return False
                elif(len(t) == 4) :
                    if((x3-x1)*(x3-x2)<=0and(y3-y1)*(y3-y2)<0 or (x3-x1)*(x3-x2)<0and(y3-y1)*(y3-y2)<=0 or (x4-x1)*(x4-x2)<=0and(y4-y1)*(y4-y2)<0 or (x4-x1)*(x4-x2)<0and(y4-y1)*(y4-y2)<=0):
                        # print 'here-->',str(line1),str(line2)
                        errorMsg = "Error:the line "+str(line1)+" and line2"+str(line2)+" overlap !\n"
                        sys.stderr.write(errorMsg)
                        return False
                    else :
                        return True

        else :
            return False


def judgeWithOhterLineSegement(street):
    for each in streets:
        for i in range(1,len(street)):
            for j in range(1,len(each)):
                if(judgeTwoLines(each[j],street[i])==False):
                    return False
    return True

def isExistInStreets(name):
    for each in streets:
        if(each[0]==name):
            return True
    return False


def add(name,pois):
    global  lineCount
    global  pointCount
    street = []
    street.append(name)
    points = pois.split(')')
    for i in range(0,len(points)-2):
        line = []
        linename = 'line'+str(lineCount)
        lineCount = lineCount + 1
        startpoint = []
        spx= float(points[i][1:len(points[i])].split(',')[0])
        spy= float(points[i][1:len(points[i])].split(',')[1])
        startpoint.append(str(spx))
        startpoint.append(str(spy))
        if(pointID.get(str(spx)+','+str(spy))==None):
            pointID[str(spx)+','+str(spy)]=pointCount
            pointCount=pointCount+1
        endpoint = []
        epx= float(points[i+1][1:len(points[i+1])].split(',')[0])
        epy= float(points[i+1][1:len(points[i+1])].split(',')[1])
        endpoint.append(str(epx))
        endpoint.append(str(epy))
        if(pointID.get(str(epx)+','+str(epy))==None):
            pointID[str(epx)+','+str(epy)]=pointCount
            pointCount=pointCount+1
        line.append(linename)
        if(startpoint == endpoint):
            errorMsg = "Error:the line has the same point !"+startpoint+"\n"
            sys.stderr.write(errorMsg)
            return
        line.append(startpoint)
        line.append(endpoint)
        street.append(line)
    ''' judge street is a reasonable street'''
#    if(judge(street) and judgeWithOhterLineSegement(street)):
        # print 'he'
    get_intersectionPoint(street)
    streets.append(street)

def print_streets():
    for each in streets:
        print each




def execute2(line):
    m = re.match(r'([acr\s]*)\"(.+)\"([0-9\s\(\)\,-]*)',line)
    if (m != None):
        numberlists = filter(None,re.split(r'[\,\(\)\s]+',m.group(3)))
        if (re.match(r'\s*[a]\s*',m.group(1))) :
            if(isExistInStreets(m.group(2))):
                sys.stderr.write("Error: 'a' specified for a street that already exists.\n")
                return
            if re.match(r'^\s*(\s*\(\s*-?\d+\s*,\s*-?\d+\s*\)\s*)+\s*$',m.group(3)) :
                if(len(numberlists)<4):
                    errorMsg = 'Error: Incomplete coordinates in a "'+m.group(2)+'", no end point.\n'
                    sys.stderr.write(errorMsg)
                    return
                else:
                    add(m.group(2),m.group(3).replace(' ',''))
            else :
                sys.stderr.write('Error: your points input format is wrong!\n')
                return
        elif(re.match(r'\s*[c]\s*',m.group(1))):
                if(isExistInStreets(m.group(2)) == False):
                    sys.stderr.write("Error: 'c' specified for a street that does not exist.\n")
                    return
                if re.match(r'\s*(\s*\(\s*-?\d+\s*,\s*-?\d+\s*\)\s*)+\s*',m.group(3)) :
                    if(len(numberlists)<4):
                        errorMsg = 'Error: Incomplete coordinates in c "'+m.group(2)+'", no end point.\n'
                        sys.stderr.write(errorMsg)
                        return
                    else:
                        remove_street(m.group(2))
                        add(m.group(2),m.group(3).replace(' ',''))
                else :
                    sys.stderr.write('Error: no points input for the street!\n')
        elif(re.match(r'\s*[r]\s*',m.group(1))):
            # print 'len-->',len(numberlists),m.group(3)
            if (isExistInStreets(m.group(2)) == False):
                sys.stderr.write("Error: 'r' specified for a street that does not exist.\n")
                return
            elif(len(numberlists)>0) :
                errorMsg = 'Error: Incomplete coordinates in r "'+m.group(2)+'",cannot have coordinates.\n'
                sys.stderr.write(errorMsg)
                return
            else :
                remove_street(m.group(2))

    else:
        t = re.match(r'(\s*[g]\s*)(\w*)',line)
        if(t == None):
            errorMsg = "Error: '"+line.split(" ")[0]+"' is not a command !\n"
            sys.stderr.write(errorMsg)
            return
        elif(re.match(r'\s*[g]\s*',line)) :
            print_graph()





while(True):
    line  = sys.stdin.readline()
    if(line.replace(' ','').replace('\n','')!=''):
        execute2(line)
    else:
        pass
        # sys.stderr.write('Error: your should input somthing!\n')