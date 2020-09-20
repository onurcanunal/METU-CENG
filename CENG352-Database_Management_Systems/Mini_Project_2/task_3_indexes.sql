create index authoredIndex311 on authored using btree(author_id);
create index authorIndex312 on author using btree(author_id);

create index authorIndex321 on author using btree(author_id,"name");
create index authoredIndex322 on authored using btree(author_id,pub_id);
create index articleIndex323 on article using btree(pub_id,journal);

create index authoredIndex351 on authored using btree(pub_id, author_id);
create index authorIndex352 on author using btree(author_id, "name");