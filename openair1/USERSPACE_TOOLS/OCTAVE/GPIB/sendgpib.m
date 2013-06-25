function [] = sendgpib(addr, command);
% Sample file showing how to interface with gpibcmd
% Command in Octave: sendgpib (20, '"FREQ 5000"')
% Must have the quotes around the command to work
% addr = 20;
% command = '"FREQ 2000"';

% this could be rewritten as native octave, see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions

cmdstr= ["./gpibcmd " sprintf('%i' , addr) " '" command "'"];
disp(['sendgpib: executing >> ' cmdstr ' <<'])
system(cmdstr);
