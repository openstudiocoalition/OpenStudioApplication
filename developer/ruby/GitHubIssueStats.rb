require 'github_api'
require 'date'
require 'yaml'

# Get the begin date, by finding the last major/minor (X.Y.0) release
# that is at least a week old
def get_begin_date_and_previous_tag()
  a_week_ago = Time.now - (60*60*24*7)

  @github.repos.releases.list(owner: @repo_owner,
                              repo: @repo).each_page do |page|
    page.each do |release|
      next if release.tag_name !~ /^v\d\.\d\.0$/
      release_date = Time.parse(release.created_at)
      next if release_date > a_week_ago
      # This is perhaps unecessary since we match to a tag in vX.Y.0 format
      # already but it doesn't hurt
      next if release.prerelease
      next if release.draft
      STDERR.puts "Found previous major/minor release: #{release.tag_name}, #{release_date}"
      return release_date, " (#{release.tag_name})"
    end
  end
  STDERR.puts "Cannot find previous release, setting time to 2005"
  return Time.new(2005, 01, 01), ""
end

def get_num(issue)
  issue.html_url.split('/')[-1].to_i
end

def get_issue_num(issue)
  "\##{get_num(issue)}"
end

def get_html_url(issue)
  issue.html_url
end

def get_title(issue)
  issue.title
end

def print_issue(issue)
  is_feature = false
  issue.labels.each {|label| is_feature = true if label.name == "Enhancement Request"}

  if is_feature
    "- :heavy_plus_sign: [#{get_issue_num(issue)}]( #{get_html_url(issue)} ), #{get_title(issue)}"
  else
    "- :heavy_check_mark: [#{get_issue_num(issue)}]( #{get_html_url(issue)} ), #{get_title(issue)}"
  end
end


@repo_owner = 'openstudiocoalition'
@repo = 'OpenStudioApplication'
@end_date = Time.now
@github = nil

if !ENV['GITHUB_TOKEN'].nil?
  token = ENV['GITHUB_TOKEN']
  @github = Github.new oauth_token: token
elsif File.exists?(Dir.home + '/github_config.yml')
  github_options = YAML.load_file(Dir.home + '/github_config.yml')
  token = github_options['oauth_token']
  @github = Github.new oauth_token: token
else
  STDERR.puts "Github Token not found"
  @github = Github.new
end

@begin_date, @prev_tag = get_begin_date_and_previous_tag()

totalOpenIssues = Array.new
totalOpenPullRequests = Array.new
newIssues = Array.new
closedIssues = Array.new
acceptedPullRequests = Array.new

# Process Open Issues
results = -1
page = 1
while (results != 0)
  resp = @github.issues.list user: @repo_owner, repo: @repo,
                            :sort => 'created',
                            :direction => 'asc',
                            :state => 'open',
                            :per_page => 100,
                            :page => page
  results = resp.length
  resp.env[:body].each do |issue, index|
    created = Time.parse(issue.created_at)
    if !issue.has_key?(:pull_request)
      totalOpenIssues << issue
      if created >= @begin_date && created <= @end_date
        newIssues << issue
      end
    else
      totalOpenPullRequests << issue
    end
  end

  page = page + 1
end

# Process Closed Issues
results = -1
page = 1
while (results != 0)
  resp = @github.issues.list user: @repo_owner, repo: @repo,
                            :sort => 'created',
                            :direction => 'asc',
                            :state => 'closed',
                            :per_page => 100,
                            :page => page
  results = resp.length
  resp.env[:body].each do |issue, index|
    created = Time.parse(issue.created_at)
    closed = Time.parse(issue.closed_at)
    if !issue.has_key?(:pull_request)
      if created >= @begin_date && created <= @end_date
        newIssues << issue
      end
      if closed >= @begin_date && closed <= @end_date
        closedIssues << issue
      end
    elsif closed >= @begin_date && closed <= @end_date
      acceptedPullRequests << issue
    end
  end

  page = page + 1
end

closedIssues.sort! {|x,y| get_num(x) <=> get_num(y)}
newIssues.sort! {|x,y| get_num(x) <=> get_num(y)}
acceptedPullRequests.sort! {|x,y| get_num(x) <=> get_num(y)}
totalOpenPullRequests.sort! {|x,y| get_num(x) <=> get_num(y)}

puts "\n**Date Range: #{@begin_date.to_date.iso8601}**#{@prev_tag} - **#{@end_date.to_date.iso8601}:**"

puts "\n**New Issues: #{newIssues.length}**"
puts "\n**Closed Issues: #{closedIssues.length}**"
puts "\n**Accepted Pull Requests: #{acceptedPullRequests.length}**"
puts "\n**Total Open Issues: #{totalOpenIssues.length}**"
puts "\n**Total Open Pull Requests: #{totalOpenPullRequests.length}**"


puts "\n\n<details>"
puts "\n  <summary>Click to see details</summary>"

puts "\n**Closed Issues: #{closedIssues.length}**"  # (" + closedIssues.map{|issue| get_issue_num(issue)}.join(', ') + ')'
closedIssues.each{|issue| puts print_issue(issue)}

puts "\n**Accepted Pull Requests: #{acceptedPullRequests.length}**" # (" + acceptedPullRequests.map{|issue| get_issue_num(issue)}.join(', ') + ')'
acceptedPullRequests.each{|issue| puts print_issue(issue)}

puts "\n**Total Open Issues: #{totalOpenIssues.length}** (" + totalOpenIssues.map{|issue| get_issue_num(issue)}.join(', ') + ')'
puts "\n**Total Open Pull Requests: #{totalOpenPullRequests.length}** (" + totalOpenPullRequests.map{|issue| get_issue_num(issue)}.join(', ') + ')'
puts "\n</details>"

