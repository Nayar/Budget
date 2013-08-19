/* METADATA TABLE */
CREATE TABLE IF NOT EXISTS metadata (
	name TEXT PRIMARY KEY,
	value TEXT
);

INSERT INTO metadata VALUES ('database_version', '1');

/* MONTH TABLE */
CREATE TABLE IF NOT EXISTS month (
	id INTEGER PRIMARY KEY,
	month INTEGER,
	year INTEGER,
	UNIQUE (month, year)
);

/*INSERT INTO month VALUES (NULL, 5, 2013);*/

/* EXPENSE TABLE */
CREATE TABLE IF NOT EXISTS expense (
	id INTEGER PRIMARY KEY,
	name TEXT,
	amount REAL,
	imagepath TEXT,
	month INTEGER REFERENCES month (id) ON DELETE CASCADE
);

/*INSERT INTO expense VALUES (NULL, 'Rent', 400.00, 'asset:///images/default.png');*/

/* INCOME TABLE */
CREATE TABLE IF NOT EXISTS income (
	id INTEGER PRIMARY KEY,
	name TEXT,
	amount REAL,
	imagepath TEXT,
	month INTEGER REFERENCES month (id) ON DELETE CASCADE
);

/*INSERT INTO income VALUES (NULL, 'BlackBerry', 3340, 'asset:///images/default.png');*/

/*
	VERSION 2
*/