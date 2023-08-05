import os, sys, glob, argparse

default_output_file = "../htmls.h"

def minimize_html(html_file, html_variable_name):
    output = f'const char {html_variable_name}[] PROGMEM = R"('
    with open(html_file, "r") as in_file:
        for line in in_file:
            line = line.strip()
            if len(line) == 0:
                continue
            output += line + '\r\n'
    output += ')";\r\n'

    return output

def generate_h(root_dir, outfile):
    html_files = glob.glob('*.html', root_dir=root_dir)
    with open(outfile, 'wb') as out:
        for html_file in html_files:
            split_file_name = os.path.splitext(html_file)
            html_variable_name = split_file_name[0] + "_html"
            print(html_file + ' => ' + html_variable_name)
            min_html = minimize_html(html_file, html_variable_name)
            out.write(min_html.encode())
    print(f'{len(html_files)} files processed.')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert all .html files in the specified folder to raw strings and save them in the specified file.')
    parser.add_argument('--path', type=str, help='Folder/path to get the files from.  Default is the current directory.')
    parser.add_argument('--out', type=str, help='File to write to.  Default is "../html.h"')
    args = parser.parse_args()

    if args.path is None:
        args.path='./'

    if args.out is not None:
        outfile = args.out
    else:
        outfile = default_output_file

    generate_h(args.path, outfile)
   
