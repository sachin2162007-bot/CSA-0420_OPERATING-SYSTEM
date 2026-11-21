-- Runs once, the first time the PostgreSQL data volume is empty
-- (mounted at /docker-entrypoint-initdb.d/init.sql by docker-compose.yml).
CREATE TABLE IF NOT EXISTS results (
    reg_no  VARCHAR(12) PRIMARY KEY,
    course  VARCHAR(10) NOT NULL,
    grade   CHAR(1)     NOT NULL
);
INSERT INTO results (reg_no, course, grade) VALUES
    ('192525001', 'CSA04', 'A'),
    ('192525002', 'CSA04', 'B'),
    ('192525045', 'CSA04', 'A')
ON CONFLICT DO NOTHING;
