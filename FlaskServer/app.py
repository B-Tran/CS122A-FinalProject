from flask import Flask, render_template
from flask import Response
from flask import jsonify
from flask import json
from flask import make_response

#create app
app = Flask(__name__)

#global variable that contains the list of notifications
List_Data = ["","","",""]

#renders the home page for notifications
@app.route('/')
def index():
    return render_template('index.html')

#gets sensor data from the home base and updates the list
@app.route('/update/<string:SensorData>')
def update(SensorData):
    List_Data[3] = List_Data[2]
    List_Data[2] = List_Data[1]
    List_Data[1] = List_Data[0]
    List_Data[0] = "%s" % (SensorData)
    return jsonify(List_Data)

#generator to make a json of the list to send to the front end
def GenerateList():
    json_data = json.dumps(
            {'data1':List_Data[0] , 'data2':List_Data[1] , 'data3':List_Data[2], 'data4':List_Data[3]})
    yield f"data:{json_data}\n\n"

#server sends data when the webpage requests for it
@app.route('/GetData1')
def GetData1():
    return Response(GenerateList(), mimetype='text/event-stream')

#main broadcast to a local server
if __name__=='__main__':
    app.run(debug=True, host='0.0.0.0')
