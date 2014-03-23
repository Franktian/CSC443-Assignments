import os
import csv
import codecs

DATADIR = "data"
OUTPUT = "parsed.list.txt"

def unicode_csv_reader(unicode_csv_data, dialect=csv.excel, **kwargs):
    # csv.py doesn't do Unicode; encode temporarily as UTF-8:
    csv_reader = csv.reader(utf_8_encoder(unicode_csv_data),
                            dialect=dialect, **kwargs)
    for row in csv_reader:
        # decode UTF-8 back to Unicode, cell by cell:
        yield [unicode(cell, 'utf-8') for cell in row]

def utf_8_encoder(unicode_csv_data):
    for line in unicode_csv_data:
        yield line.encode('utf-8')

output = codecs.open(OUTPUT, 'w', 'utf-8')
for f in os.listdir(DATADIR):
	_, extension = os.path.splitext(f)
	if extension != ".csv":
		continue
	print "processing csv file " + f
	filename = os.path.join(DATADIR, f)
	with codecs.open(filename, 'rb', 'utf-8') as csvfile:
		reader = unicode_csv_reader(csvfile, skipinitialspace=True)
		for i, row in enumerate(reader):
			if i == 0:
				continue
			[output.write(txt.lstrip().rstrip()+u"\n") for txt in row]
			output.write(u"\n")
output.close()