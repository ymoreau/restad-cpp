#!/usr/bin/env ruby
#
# TEST RESTAD
#
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
# Author : Yoann Moreau (moreau.yo * at * gmail.com)
#
# Copyright (C) 2012 LIA (www.lia.univ-avignon.fr)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see 
# <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>.
#

require 'pg'

# Command line arg = config file for Restad
if ARGV.size < 1
  STDERR.puts "Needs restad config file as argument\ntest_index CONFIG_FILE"
  exit
end
config_file = ARGV[0]

# Parse for database config
config_str = File.open(config_file, "r").read
dbhost = config_str.match(/dbhost\s*=\s*((\w)+)/)[1]
dbname = config_str.match(/dbname\s*=\s*((\w)+)/)[1]
dbuser = config_str.match(/dbuser\s*=\s*((\w)+)/)[1]
dbpassword = ""
match_dbpassword = config_str.match(/dbpassword\s*=\s*((\w)+)/)
dbpassword = match_dbpassword[1] unless match_dbpassword.nil?
if dbpassword.empty?
  print "Database password: "
  dbpassword = STDIN.gets
  dbpassword.chomp!
end

db = PGconn.connect("host=#{dbhost} dbname=#{dbname} user=#{dbuser} password=#{dbpassword}")

# Tests
puts "PREPARSING"
begin
# Add the all-in sample file
  `./preparser -f #{config_file} -w "#{dbpassword}" -m document #{File.expand_path(File.dirname(__FILE__))}/corpus`
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%all_in_sample.xml'")
  raise "No all_in_sample.xml file in the database" if res.ntuples < 1
  raise "More than 1 result for all_in_sample.xml" if res.ntuples > 1
  raise "Wrong doc tag for all_in_sample.xml '#{res[0]['doc_tag']}'" if res[0]['doc_tag'] != "document"
  raise "Wrong status for all_in_sample.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 0
  puts "  all_in_sample.xml ok"

# Add the sample files
  `./preparser -f #{config_file} -w "#{dbpassword}" -r #{File.expand_path(File.dirname(__FILE__))}/corpus/docs`
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample1.xml'")
  raise "No sample1.xml file in the database" if res.ntuples < 1
  raise "More than 1 result for sample1.xml" if res.ntuples > 1
  raise "Wrong doc tag for sample1.xml '#{res[0]['doc_tag']}'" if res[0]['doc_tag'] != nil
  raise "Wrong status for sample1.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 0
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample2.xml'")
  raise "No sample2.xml file in the database" if res.ntuples < 1
  raise "More than 1 result for sample2.xml" if res.ntuples > 1
  raise "Wrong doc tag for sample2.xml '#{res[0]['doc_tag']}'" if res[0]['doc_tag'] != nil
  raise "Wrong status for sample2.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 0
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample_error.xml'")
  raise "No sample_error.xml file in the database" if res.ntuples < 1
  raise "More than 1 result for sample_error.xml" if res.ntuples > 1
  raise "Wrong doc tag for sample_error.xml '#{res[0]['doc_tag']}'" if res[0]['doc_tag'] != nil
  raise "Wrong status for sample_error.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 0
  puts "  sample*.xml ok"
rescue => msg
  STDERR.puts "#{msg.backtrace[0]} : #{msg}"
  puts "...preparsing failed, stopping the test."
  exit
end

puts "INDEXING"
begin
  `./indexer -f #{config_file} -w "#{dbpassword}" -q`
# Check the file status
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%all_in_sample.xml'")
  raise "No all_in_sample.xml file in the database" if res.ntuples < 1
  raise "Wrong status for all_in_sample.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 1
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample1.xml'")
  raise "No sample1.xml file in the database" if res.ntuples < 1
  raise "Wrong status for sample1.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 1
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample2.xml'")
  raise "No sample2.xml file in the database" if res.ntuples < 1
  raise "Wrong status for sample2.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 1
  res = db.exec("SELECT * FROM files WHERE uri LIKE '%sample_error.xml'")
  raise "No sample_error.xml file in the database" if res.ntuples < 1
  raise "Wrong status for sample_error.xml '#{res[0]['status']}'" if res[0]['status'].to_i != 0
  puts "  Status ok"

# Check the docs
  res = db.exec("SELECT * FROM docs NATURAL JOIN files WHERE uri LIKE '%all_in_sample.xml'")
  raise "Wrong number of documents for all_in_sample.xml '#{res.ntuples}'" if res.ntuples != 2
  res = db.exec("SELECT * FROM docs NATURAL JOIN files WHERE uri LIKE '%sample1.xml'")
  raise "Wrong number of documents for sample1.xml '#{res.ntuples}'" if res.ntuples != 1
  res = db.exec("SELECT * FROM docs NATURAL JOIN files WHERE uri LIKE '%sample2.xml'")
  raise "Wrong number of documents for sample2.xml '#{res.ntuples}'" if res.ntuples != 1
  res = db.exec("SELECT * FROM docs NATURAL JOIN files WHERE uri LIKE '%sample_error.xml'")
  raise "Wrong number of documents for sample_error.xml '#{res.ntuples}'" if res.ntuples != 0
  puts "  Docs ok"
rescue => msg
  STDERR.puts "#{msg.backtrace[0]} : #{msg}"
end

