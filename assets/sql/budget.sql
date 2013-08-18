CREATE TABLE IF NOT EXISTS metadata (
	name TEXT PRIMARY KEY,
	value TEXT
);

INSERT INTO metadata VALUES ('database_version', '1');

CREATE TABLE IF NOT EXISTS expense (
	id INTEGER PRIMARY KEY,
	name TEXT,
	amount REAL,
	imagepath TEXT
);

INSERT INTO expense VALUES (NULL, 'Rent', 400.00, 'asset:///images/default.png');

CREATE TABLE IF NOT EXISTS income (
	id INTEGER PRIMARY KEY,
	name TEXT,
	amount REAL,
	imagepath TEXT
);

INSERT INTO income VALUES (NULL, 'BlackBerry', 3340, 'asset:///images/default.png');

/*
	VERSION 2 
*/

CREATE TABLE IF NOT EXISTS month (
	id INTEGER PRIMARY KEY,
	month INTEGER,
	year INTEGER,
	UNIQUE (month, year)
);

INSERT INTO month VALUES (NULL, 1, 2013);
INSERT INTO month VALUES (NULL, 10, 2012);

ALTER TABLE expense
ADD COLUMN month INTEGER REFERENCES month (id) ON DELETE CASCADE;

ALTER TABLE income
ADD COLUMN month INTEGER REFERENCES month (id) ON DELETE CASCADE;