import re
import sys


def colorize(lines):
    def bold(s):
        return u'\u001b[1m' + s + '\u001b[0m'

    def red(s):
        return u'\u001b[31m' + s + '\u001b[0m'

    def green(s):
        return u'\u001b[32m' + s + '\u001b[0m'

    def yellow(s):
        return u'\u001b[33m' + s + '\u001b[0m'

    def blue(s):
        return u'\u001b[34m' + s + '\u001b[0m'

    def magenta(s):  # purple
        return u'\u001b[35m' + s + '\u001b[0m'

    def cyan(s):
        return u'\u001b[36m' + s + '\u001b[0m'

    def format_severity(txt, severity):
        """
        http://cppcheck.sourceforge.net/devinfo/doxyoutput/classSeverity.html
        enum:
            none, error, warning, style, performance,
            portability, information, debug
        """
        if severity == "none":
            return txt
        if severity == "error":
            return red(txt)
        if severity == "warning":
            return yellow(txt)
        if severity == 'style':
            return blue(txt)
        if severity == "performance":
            return cyan(txt)
        if severity == "portability":
            return magenta(txt)
        if severity == "information":
            return green(txt)
        if severity == "debug":
            return txt

        return txt

    re_message = re.compile(r'\[(?P<file>.*):(?P<line>.*?)\]:\((?P<severity>.*?)\),\[(?P<id>.*?)\],(?P<message>.*)')

    colored_lines = []
    matched_messages = []

    colored_lines = []
    matched_messages = []

    for line in lines:
        m = re_message.match(line)
        if m:
            d = m.groupdict()
            matched_messages.append(d)
        else:
            colored_lines.append(red(line))

    severity_order = ['error', 'warning', 'style', 'performance',
                      'portability', 'information', 'debug', 'none']

    for d in sorted(matched_messages,
                    key=lambda d: severity_order.index(d['severity'])):

        f = d['file']
        line = d['line']
        severity = d['severity']
        iid = d['id']
        message = d['message']

        colored_lines.append(
            "[{f}:{line}]:({severity}),[{i}],{message}"
            .format(f=magenta(f),  # format_severity(f, severity),
                    line=green(line),
                    severity=format_severity(severity, severity),
                    i=bold(iid),
                    message=message))

    return colored_lines


if __name__ == '__main__':
    with open('cppcheck.txt', 'r') as f:
        content = f.read()

    lines = content.splitlines()
    colored_lines = colorize(lines)
    # sys.stdout.writelines(colored_lines)
    print("\n".join(colored_lines))
