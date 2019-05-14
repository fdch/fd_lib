import json, os, glob
import numpy as np
import struct
# import utils

# header data, which is the number of instances followed by the length of each instance

def convert(file,target="___default___"):
	if "___default___" in target:
		target=os.path.splitext(file)[0]+".json"
	instances=[]
	instances_lenghts=[]
	data={"instances":[]}
	with open(file, "rb") as f:
		num_instances=np.fromfile(f, count=1, dtype=np.uint32)	
		instances_lenghts=np.fromfile(f, count=num_instances, dtype=np.uint32)
		for i in num_instances:
			for j in instances_lenghts:
				instances=np.fromfile(f, count=j, dtype=np.float32)
				data['instances'].append(instances.tolist())
		data.update({"num_instances":num_instances.tolist()[0]})
		data.update({"instances_lenghts":instances_lenghts.tolist()})

	with open(target, "w") as f:
		f.write(json.dumps(data,indent=4)) 

def revert(file,target="___default___"):
	if "___default___" in target:
		target=os.path.splitext(file)[0]+".json"
	instances=[]
	instances_lenghts=[]
	data={}
	with open(file, "r") as f:
		data=json.load(f) 
	with open(target, "wb") as f:
		num_instances=data['num_instances']
		f.write(struct.pack("L",num_instances))
		for i in data['instances_lenghts']:
			f.write(struct.pack("L",i))
		for i in data['instances']:
			for j in i:
				f.write(struct.pack("d",j))

def convert_extra(file,target="___default___"):
	if "___default___" in target:
		target=os.path.splitext(file)[0]+".json"
	instances=[]
	instances_lenghts=[]
	data={"instances":[]}
	with open(file, "rb") as f:
		num_instances=np.fromfile(f, count=1, dtype=np.uint32)	
		instances_lenghts=np.fromfile(f, count=num_instances, dtype=np.uint32)
		num_instances=num_instances.tolist()[0]
		for i in range(num_instances):

			data['instances'].append({
				"brightness": 0,
				"body_face" : 0,
				"body_face_size" : 0,
				"blobiness" : 0,
				"boundedness": 0,
				"kontrastedness": 0,
				"histo":0
				})
			
			v=np.fromfile(f, count=1, dtype=np.float32)
			data['instances'][i]['brightness'] = float(v.tolist()[0])
			
			v=np.fromfile(f, count=1, dtype=np.float32)
			data['instances'][i]['body_face'] = float(v.tolist()[0])
			
			v=np.fromfile(f, count=1, dtype=np.float32)
			data['instances'][i]['body_face_size'] = float(v.tolist()[0])

			v=np.fromfile(f, count=3, dtype=np.float32)
			data['instances'][i]['blobiness'] = float(sum(v.tolist())/3.0)

			v=np.fromfile(f, count=2, dtype=np.float32)
			data['instances'][i]['boundedness'] = float(sum(v.tolist())/2.0)

			v=np.fromfile(f, count=1, dtype=np.float32)
			data['instances'][i]['kontrastedness'] = float(v.tolist()[0])
			
			v=np.fromfile(f, count=50, dtype=np.float32)
			data['instances'][i]['histo'] = v.tolist()
			
			v=np.fromfile(f, count=50, dtype=np.float32)

		data.update({"num_instances":num_instances})

	with open(target, "w") as f:
		f.write(json.dumps(data,indent=4)) 

def batch_convert(path,base="*.timid"):
	for file in glob.glob(os.path.join(path, base)):
		convert(file)

def batch_revert(path,base="*.json"):
	for file in glob.glob(os.path.join(path, base)):
		revert(file)

def batch_convert_extra(path,base="*.timid"):
	for file in glob.glob(os.path.join(path, base)):
		convert_extra(file)

# convert_extra("./sound_data/chocolate.timid", "test.json")
